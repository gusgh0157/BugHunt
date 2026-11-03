# third_party

직렬화 랩(`BugHunt/src/serialize`)이 쓰는 외부 라이브러리. 버전을 고정해 저장소에 넣었다. 고치지 않는다.

| 라이브러리 | 버전 | 출처 | 라이선스 | 들어 있는 파일 |
|---|---|---|---|---|
| nlohmann/json | 3.11.3 | https://github.com/nlohmann/json (태그 v3.11.3, `single_include`) | MIT (`nlohmann_json/LICENSE.MIT`) | `nlohmann_json/include/nlohmann/json.hpp` |
| pugixml | 1.14 | https://github.com/zeux/pugixml (태그 v1.14, `src`) | MIT (`pugixml/LICENSE.md`) | `pugixml/src/pugixml.hpp`, `pugixml.cpp`, `pugiconfig.hpp` |

빌드: 포함 경로 `third_party/nlohmann_json/include`, `third_party/pugixml/src`를 더하고 `pugixml.cpp`를 함께 컴파일한다.
