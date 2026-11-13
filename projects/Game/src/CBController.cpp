#include "CBController.h"

#include <UniDx/D3DManager.h>
#include <UniDx/Input.h>
#include <algorithm>

using namespace UniDx;

namespace
{
    // 定数バッファ（型定義自体はシェーダーに伝わらないことに注意）
    struct CBParam // register(b0)
    {
        Color baseColor;
        float wavePower;
        float waveSpeed;
        float padding[2];
    };

    constexpr int CBSlot = 0;

    float wavePower = 0.1f;
    float waveSpeed = 1.0f;

    constexpr float wavePowerStep = 0.1f;
    constexpr float waveSpeedStep = 0.1f;

    Color colorTable[] =
    {
        Color(1, 0, 0),
        Color(1, 1, 0),
        Color(1, 0, 1),
        Color(0, 1, 1),
        Color(0.5, 0, 0),
        Color(0, 0.5, 0),
        Color(0, 0, 0.5),
    };
    constexpr int tableSize = sizeof(colorTable) / sizeof(Color);
}


void CBController::Start()
{
    D3D11_BUFFER_DESC desc{};
    desc.ByteWidth = sizeof(CBParam);       // 構造体のサイズを指定
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;

    // 定数バッファを確保
    D3DManager::getInstance()->GetDevice()->CreateBuffer(&desc, nullptr, constantBuffer.GetAddressOf());
}


void CBController::Update()
{
    if (Input::GetKeyDown(Keyboard::Down))
    {
        colorIndex = (colorIndex + 1) % tableSize;
    }

    if (Input::GetKeyDown(Keyboard::Up))
    {
        colorIndex = (colorIndex - 1 + tableSize) % tableSize;
    }

    if (Input::GetKeyDown(Keyboard::Right))
    {
        wavePower += wavePowerStep;
        waveSpeed += waveSpeedStep;
    }

    if (Input::GetKeyDown(Keyboard::Left))
    {
        wavePower -= wavePowerStep;
        waveSpeed -= waveSpeedStep;
    }

    // 0～5の範囲に制限
    wavePower = std::clamp(wavePower, 0.0f, 2.0f);
    waveSpeed = std::clamp(waveSpeed, 0.0f, 5.0f);

    CBParam cb{};

    // 色
    cb.baseColor = colorTable[colorIndex];

    // 揺れの強さ
    cb.wavePower = wavePower;

    // 揺れる速さ
    cb.waveSpeed = waveSpeed;

    // 定数バッファを更新
    D3DManager::getInstance()->GetContext()->UpdateSubresource(constantBuffer.Get(), 0, nullptr, &cb, 0, 0);

    // シェーダーへ設定
    ID3D11Buffer* cbs[1] = { constantBuffer.Get() };
    D3DManager::getInstance()->GetContext()->VSSetConstantBuffers(CBSlot, 1, cbs);
    D3DManager::getInstance()->GetContext()->PSSetConstantBuffers(CBSlot, 1, cbs);

}