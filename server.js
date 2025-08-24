const express = require('express');
const axios = require('axios');
const CryptoJS = require('crypto-js');
const cors = require('cors');
const app = express();
const port = 3000;

// 1. 配置中间件（解决跨域、解析JSON请求）
app.use(cors());
app.use(express.json());

// 2. 豆包AI API配置（替换为你的API Key和Secret Key）
const DOUBAO_API_KEY = "你的豆包API Key";
const DOUBAO_SECRET_KEY = "你的豆包Secret Key";
const DOUBAO_API_URL = "https://open.doubao.com/api/v1/chat/completions"; // 豆包文本生成接口

// 3. 生成豆包API调用的签名（参考豆包开放平台文档）
function generateDoubaoSign(timestamp, nonce) {
  // 签名规则：将API Key、Secret Key、时间戳、随机数按指定顺序拼接，用SHA256加密
  const signStr = `${DOUBAO_API_KEY}${DOUBAO_SECRET_KEY}${timestamp}${nonce}`;
  return CryptoJS.SHA256(signStr).toString(CryptoJS.enc.Hex).toUpperCase();
}

// 4. 处理前端食材请求的接口
app.post('/api/generate-recipe', async (req, res) => {
  try {
    // 4.1 接收前端传递的食材数据
    const { ingredients } = req.body;
    if (!ingredients) {
      return res.status(400).json({ error: "请输入至少一种食材" });
    }

    // 4.2 生成豆包API请求参数（时间戳、随机数、签名）
    const timestamp = Date.now().toString(); // 当前时间戳（毫秒级）
    const nonce = Math.random().toString(36).substr(2, 10); // 随机字符串
    const sign = generateDoubaoSign(timestamp, nonce);

    // 4.3 构造豆包AI请求体（明确指令，让AI生成结构化菜谱）
    const doubaoRequest = {
      model: "doubao-pro-1.0", // 豆包AI模型（根据需求选择）
      messages: [
        {
          role: "user",
          content: `基于食材【${ingredients}】，生成1份家常菜谱，要求：
1. 菜谱名称清晰（如“西红柿炒鸡蛋”）；
2. 分“所需食材”和“烹饪步骤”两部分，步骤需详细到具体操作（如“番茄去皮切小块”）；
3. 食材用量合理，步骤不超过6步，语言简洁易懂，适合家庭烹饪；
4. 不要额外添加用户未提及的核心食材，可推荐少量常见调料（如盐、食用油）。`
        }
      ],
      temperature: 0.7, // 生成多样性（0-1，值越小越稳定）
      max_tokens: 500 // 最大生成文本长度（足够容纳菜谱内容）
    };

    // 4.4 发起豆包AI API请求
    const doubaoResponse = await axios({
      method: 'POST',
      url: DOUBAO_API_URL,
      headers: {
        "Content-Type": "application/json",
        "API-Key": DOUBAO_API_KEY,
        "Timestamp": timestamp,
        "Nonce": nonce,
        "Sign": sign
      },
      data: doubaoRequest
    });

    // 4.5 提取豆包AI返回的菜谱内容（处理响应格式）
    const aiRecipe = doubaoResponse.data.choices[0].message.content;
    if (!aiRecipe) {
      return res.status(500).json({ error: "豆包AI未生成有效菜谱" });
    }

    // 4.6 解析AI返回的结构化数据
    let recipeName = "家常菜";
    let recipeIngredients = [];
    let recipeSteps = [];

    // 简单解析逻辑（可根据AI实际返回格式调整）
    const lines = aiRecipe.split('\n').filter(line => line.trim());
    lines.forEach(line => {
      if (line.includes("菜谱名称")) {
        recipeName = line.replace("菜谱名称：", "").trim();
      } else if (line.includes("所需食材")) {
        const ingStr = line.replace("所需食材：", "").trim();
        recipeIngredients = ingStr.split("、").map(ing => ing.trim());
      } else if (line.includes("步骤") && line.match(/^\d+/)) {
        recipeSteps.push(line.trim());
      }
    });

    // 如果AI没有按预期格式返回，做备选处理
    if (recipeIngredients.length === 0 || recipeSteps.length === 0) {
      recipeName = "推荐菜谱";
      recipeIngredients = ["请参考菜谱说明中的食材"];
      recipeSteps = [aiRecipe];
    }

    // 4.7 生成B站视频链接（基于菜谱名称检索）
    const b站关键词 = `${recipeName} 家常做法 教学`;
    // 调用B站API（此处用模拟链接示例，实际需替换为真实接口）
    const b站视频链接 = `https://www.bilibili.com/search?keyword=${encodeURIComponent(b站关键词)}`;
    const b站视频标题 = `【${recipeName}】详细教学，在家轻松做`;

    // 4.8 返回整合结果（菜谱+B站链接）给前端
    res.json({
      success: true,
      data: {
        recipeName,
        recipeIngredients,
        recipeSteps,
        b站Video: {
          title: b站视频标题,
          url: b站视频链接,
          thumbnail: `https://picsum.photos/seed/${recipeName}/200/120` // 模拟视频缩略图
        }
      }
    });

  } catch (error) {
    console.error("调用豆包AI失败：", error);
    res.status(500).json({ error: "生成菜谱失败，请稍后重试" });
  }
});

// 5. 启动后端服务
app.listen(port, () => {
  console.log(`后端代理服务运行在 http://localhost:${port}`);
});
