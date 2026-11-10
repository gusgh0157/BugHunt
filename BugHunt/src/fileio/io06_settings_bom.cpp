#include "../labs.h"
#include "io_kit.h"
#include <map>
#include <string>

// 설정 파일(한 줄에 key=value)을 읽는다. 없는 키는 기본값을 쓴다.
// 플레이어가 메모장으로 음량을 30으로 고쳐 저장했다는데, 게임에서는 여전히 기본값 80이다.

// [JA] 設定ファイル(1行に key=value)を読み込みます。ないキーは既定値を使います。
// [JA] プレイヤーはメモ帳で音量を30に直して保存したそうですが、ゲームでは既定値の80のままです。
namespace io6
{
namespace fs = std::filesystem;

std::map<std::string, std::string> loadSettings(const fs::path& file)
{
    std::map<std::string, std::string> settings;
    std::ifstream in(file);
    std::string line;
    while (std::getline(in, line))
    {
        const size_t eq = line.find('=');
        if (eq == std::string::npos)
            continue;
        settings[line.substr(0, eq)] = line.substr(eq + 1);
    }
    return settings;
}

int getInt(const std::map<std::string, std::string>& settings, const std::string& key, int fallback)
{
    const auto it = settings.find(key);
    return it == settings.end() ? fallback : std::stoi(it->second);
}
}

void io06_settings_bom()
{
    using namespace io6;

    iokit::LabDir lab("io06_settings_bom");
    const fs::path file = lab / "settings.ini";
    // 메모장 등 일부 편집기는 UTF-8로 저장할 때 맨 앞에 BOM(EF BB BF) 3바이트를 붙인다.
    // [JA] メモ帳など一部のエディターは、UTF-8 で保存するときに先頭に BOM(EF BB BF) 3 バイトを付けます。
    iokit::writeBytes(file, "\xEF\xBB\xBF" "volume=30\nfullscreen=1\nlanguage=ko\n");

    const auto settings = loadSettings(file);
    std::cout << i18n::L("  읽은 설정 ", "  読み込んだ設定") << settings.size() << i18n::L("개\n", "個\n");

    expect(i18n::L("음량", "音量"), 30, getInt(settings, "volume", 80));
    expect(i18n::L("전체 화면", "全画面"), 1, getInt(settings, "fullscreen", 0));
}

// ============================================================================
// 힌트: 막힐 때 하나씩만 펼치세요. (Ctrl+M, Ctrl+O = 전부 접기)
// [JA] ヒント: 行き詰まったときに一つずつ開いてください。(Ctrl+M, Ctrl+O = すべて折りたたむ)
// ============================================================================

#pragma region 힌트 1 / ヒント 1 (먼저 직접 추적해 보고 펼치세요 / まず自分で追跡してから開いてください)
// getInt에 조건부 중단점을 거세요. 조건: key == "volume"
// 멈추면 settings를 펼쳐 키 목록을 보세요. "volume"처럼 보이는 키가 있는데 왜 못 찾을까요?

// [JA] getInt に条件付きブレークポイントを設定してください。条件: key == "volume"
// [JA] 止まったら settings を展開してキーの一覧を見てください。"volume" に見えるキーがあるのに、なぜ見つからないのでしょうか?
#pragma endregion

#pragma region 힌트 2 / ヒント 2
// 보이는 글자만 믿지 마세요. 조사식에 settings.begin()->first.size() 를 넣거나,
// loadSettings의 첫 줄에서 line.size()와 (int)(unsigned char)line[0] 을 보세요. "volume"은 6글자입니다.

// [JA] 見えている文字だけを信じないでください。ウォッチに settings.begin()->first.size() を入れるか、
// [JA] loadSettings の1行目で line.size() と (int)(unsigned char)line[0] を見てください。"volume" は6文字です。
#pragma endregion

#pragma region 힌트 3 / ヒント 3
// 파일 맨 앞에 BOM(EF BB BF)이 있으면 첫 줄에서만 그 3바이트를 떼어 내세요.
// BOM이 없는 파일도 많으므로 "무조건 3바이트 버리기"는 안 됩니다.

// [JA] ファイルの先頭にBOM(EF BB BF)があるときだけ、1行目からその3バイトを取り除いてください。
// [JA] BOMのないファイルも多いので、「無条件に3バイト捨てる」はいけません。
#pragma endregion

#pragma region 채점용 숨은 테스트 / 採点用の隠しテスト (풀이에는 필요 없습니다. 고치지 마세요 / 解答には不要です - 変更しないでください)
// 채점기(grade.bat)가 화면에 나오지 않는 다른 데이터로 같은 코드를 한 번 더 검사한다.
// 기대값을 하드코딩해서 [PASS]를 만들면 여기서 걸린다.
// [JA] 採点器(grade.bat)が、画面には表示されない別のデータで同じコードをもう一度検査します。
// [JA] 期待値をハードコードして [PASS] にしても、ここで引っかかります。
void io06_settings_bom_hidden()
{
    using namespace io6;

    iokit::LabDir lab("io06_settings_bom_hidden");

    iokit::writeBytes(lab / "bom.ini", "\xEF\xBB\xBF" "fov=95\nvolume=12\n");
    const auto withBom = loadSettings(lab / "bom.ini");
    expect(i18n::L("BOM 있음: 시야각", "BOMあり: 視野角"), 95, getInt(withBom, "fov", 60));
    expect(i18n::L("BOM 있음: 음량", "BOMあり: 音量"), 12, getInt(withBom, "volume", 80));

    iokit::writeBytes(lab / "plain.ini", "brightness=7\nvolume=55\n");   // BOM 없음 [JA] BOM なし
    const auto plain = loadSettings(lab / "plain.ini");
    expect(i18n::L("BOM 없음: 밝기", "BOMなし: 明るさ"), 7, getInt(plain, "brightness", 5));
    expect(i18n::L("BOM 없음: 음량", "BOMなし: 音量"), 55, getInt(plain, "volume", 80));
}
#pragma endregion
