// ----------------------------------------------------------
// テクスチャ１枚、環境光、ディレクショナルライト、
// マテリアルカラーだけの単純な陰影シェーダー
// ----------------------------------------------------------

// 今回追加した定数バッファ
cbuffer CBParam : register(b0)
{
    float4 baseColor;
};

// ----------------------------------------------------------
// 頂点シェーダ
// ----------------------------------------------------------
// カメラ定数バッファ
cbuffer VSConstants : register(b8)
{
    float4x4 view;
    float4x4 projection;
};

// 行列定数バッファ
cbuffer VSConstants : register(b9)
{
    float4x4 world;
};

// 頂点シェーダーへ入力するデータ
struct VSInput
{
    float3 pos : POSITION;
    float3 nrm : NORMAL;
};

// 頂点シェーダーから出力するデータ
struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
};

float4 main(PSInput input) : SV_TARGET
{
    // 法線を正規化
    float3 N = normalize(input.normal);

// ライトの方向
float3 L = normalize(float3(0.0f, 1.0f, -1.0f));

// 拡散光の計算
float diffuse = max(0.0f, dot(N, L));

// 拡散光の色
float3 diffuseColor = float3(1.0f, 1.0f, 1.0f) * diffuse;

// ピクセルカラー
float3 pixelColor = diffuseColor;

return float4(pixelColor, 1.0f);
}

// 頂点シェーダー
PSInput VS(VSInput vin)
{
    PSInput Out;

    float4 p = float4(vin.pos.xyz, 1);
    p = mul(world, p); // ワールド変換
    p = mul(view, p); // ビュー変換
    p = mul(projection, p); // プロジェクション変換
    Out.posH = p;

    float3x3 world3x3 = (float3x3) world;
    Out.nrmW = mul(world3x3, vin.nrm);
    return Out;
}


// ----------------------------------------------------------
// ピクセルシェーダ
// ----------------------------------------------------------
// 共通ライト
cbuffer LightPerFrame : register(b11)
{
    float4 ambientColor;
    float4 directionalColor;
    float3 directionW;
    uint pad;
};

// ピクセルシェーダー
float4 PS(PSInput In) : SV_Target0
{
    // 明示的に法線を正規化（モデルスケール非均等だと崩れるため）
    float3 N = normalize(In.nrmW);

    // ライトループ
    float4 diffAccum = ambientColor; // 環境光

    // カラー合成
    float4 color = float4(diffAccum.rgb, 1) * baseColor;

    // テクスチャの色を出力
    return color;
}
