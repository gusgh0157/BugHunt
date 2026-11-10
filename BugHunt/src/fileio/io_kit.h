#pragma once
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>
#include <string_view>

// 파일 입출력 랩 공용 도구 (버그 없음)
// 모든 파일은 임시 폴더/BugHunt/<랩 이름> 아래에만 만들고, 랩이 끝나면 통째로 지운다.
// 채점기는 랩마다 프로세스를 따로 띄우므로 랩끼리, 그리고 평소 실행과 숨은 테스트끼리 폴더 이름이 겹치지 않게 한다.

// [JA] ファイル入出力ラボ共通のツールです(バグはありません)。
// [JA] ファイルはすべて 一時フォルダー/BugHunt/<ラボ名> の下だけに作り、ラボが終わったらフォルダーごと削除します。
// [JA] 採点ツールはラボごとに別プロセスを起動するので、ラボ同士、通常実行と隠しテスト同士でフォルダー名が重ならないようにします。
namespace iokit
{
namespace fs = std::filesystem;

class LabDir
{
public:
    explicit LabDir(const char* name)
        : path_(fs::temp_directory_path() / "BugHunt" / name)
    {
        std::error_code ec;
        fs::remove_all(path_, ec);      // 지난번에 중간에 멈춘 실행이 남긴 것 [JA] 前回の途中で止まった実行が残したもの
        fs::create_directories(path_);
    }
    ~LabDir()
    {
        std::error_code ec;
        fs::remove_all(path_, ec);
    }
    LabDir(const LabDir&) = delete;
    LabDir& operator=(const LabDir&) = delete;

    const fs::path& path() const { return path_; }
    fs::path operator/(const fs::path& relative) const { return path_ / relative; }

private:
    fs::path path_;
};

// 연습용 파일을 바이트 그대로 만든다 (줄바꿈 변환 없음). 필요하면 폴더도 만든다.
// [JA] 練習用のファイルをバイトそのままで作ります(改行変換なし)。必要ならフォルダーも作ります。
inline void writeBytes(const fs::path& file, std::string_view bytes)
{
    fs::create_directories(file.parent_path());
    std::ofstream out(file, std::ios::binary);
    out.write(bytes.data(), static_cast<std::streamsize>(bytes.size()));
}

inline std::string readBytes(const fs::path& file)
{
    std::ifstream in(file, std::ios::binary);
    return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}
}
