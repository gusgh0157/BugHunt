[한국어](SOLUTIONS.md) | プログラムの実行出力は `--lang ja` で日本語になります(ラボのコードが独自に出す進行メッセージも含みます。ゲーム内のデータと、ラボが保存して比較する文言は韓国語のままです)。本書では日本語に訳して示し、デバッガーに入力する文字列、バイト単位で見る値、解答コードの文字列リテラルだけは原文の韓国語を残して(　)やコメントで訳を添えています。

# 解答(ネタバレ注意!)

第1部~第12部の解答です。チャレンジ(C、B)の解答は `CHALLENGE_SOLUTIONS.ja.md` に別にあります。

# 第1部: 呼び出し履歴入門

## Lab 1 — nullptr の参照外し
```
止まった関数: printPlayer           おかしな値: player == nullptr
値の出どころ: showProfile ← findPlayer が見つけられず nullptr を返した
本当の原因:   openFriendList で "Doran" を "Dorna" と打ち間違えた
```
**修正:** 打ち間違いを直します。防御も追加: `if (!player) { std::cout << "  " << name << ": 없는 플레이어\n"; return; }`(存在しないプレイヤー)
**教訓:** 落ちた場所(printPlayer)には何の罪もありません。nullptr を **受け取って検査しなかった** 場所と、**間違った名前を渡した** 場所が原因です。

## Lab 2 — 1から数えるキーを0から数えるインデックスとしてそのまま使用
```
止まった関数: [外部コード] std::out_of_range を投げる ← vector::at → 最初の自分のコード: QuickSlotBar::itemAt(slotIndex = 4)
値の出どころ: onNumberKey(key = 4) ← replayInput
本当の原因:   キー番号(1~4)をスロットインデックス(0~3)に変換していない
```
**修正:** `useItem(bar.itemAt(key - 1));`
**教訓:** 外部コードは飛ばして、自分のコードの最初の行から見ます。そして、クラッシュの前に出た「間違った出力」も同じバグの症状です。

## Lab 3 — 整数の割り算で0秒になる
```
止まった関数: damagePerSecond(seconds = 0) が std::invalid_argument を投げる(例外がスローされたときに中断する設定をオンにすると throw 行で止まる)
値の出どころ: printBattleSummary: battle = {"Slime", 35, 800}, toSeconds(800) == 0
本当の原因:   1秒未満の戦闘が整数の割り算で0秒になる
```
**修正:** ミリ秒で計算: `totalDamage * 1000 / durationMs`(そして `durationMs == 0` の検査)、または `double` で計算。
**教訓:** 値の出どころを何フレームもさかのぼる必要があります。0で割った関数は、受け取った値を使っただけです。

## Lab 4 — 検査が片方の経路にしかない
```
止まった関数: leaderOf ← sendRaidReminders  (printPartyBoard ではない!)
おかしな値:   party.name == "Silent Hollow"、members が空
```
**修正:** `sendRaidReminders` にも `if (party.members.empty()) continue;`。さらに良い方法は、`leaderOf` が空のパーティーを安全に扱うようにすることです(`const std::string*` を返す、`std::optional` など)。
**教訓:** 同じ関数でも **どの経路で呼ばれたか** は、呼び出し履歴だけが正確に教えてくれます。

## Lab 5 — 終了条件を飛び越える再帰
```
止まった関数: countdown(n = マイナス数万) が数万回繰り返される → スタックオーバーフロー
値の出どころ: n が奇数 → startFireworks(seconds = 5)
本当の原因:   5, 3, 1, -1, ... と0を飛び越えて終わらない
```
**修正:** `if (n <= 0) return i18n::L("발사!", "発射!");`
**教訓:** スタックが同じ関数で埋まっていたら、再帰が終わっていないということです。引数の値の変化を見れば理由がわかります。

## Lab 6 — 存在しないキーで map::at
```
止まった関数: [外部コード] std::out_of_range を投げる ← std::map::at → 最初の自分のコード: priceOf(itemId = "mana_poiton")
値の出どころ: cartTotal ← checkout のカート一覧
本当の原因:   "mana_potion" の打ち間違い
```
**修正:** 打ち間違いを直します。防御: `auto it = kPrices.find(itemId); if (it == kPrices.end()) { /* 警告して0 */ }`
**教訓:** 例外も呼び出し履歴で追跡します。throw した場所 → 最初に出てくる自分のコード。

## Lab 7 — 早すぎるタイミングでキャプチャしたポインター
```
止まった関数: HealthBar::setValue (this == nullptr)
下のフレーム: connectHealthBar のラムダ — キャプチャされた bar == nullptr
          ← std::function (外部コード) ← EventBus::publishHpChanged ← startGame
本当の原因:   startGame で connectHealthBar(game) を createUi(game) より先に呼んでいる。
           その時点で game.healthBar.get() は nullptr で、ラムダはその値をコピーして保存した。
```
**修正:** `createUi(game);` を `connectHealthBar(game);` の上に移します。または、ラムダが `&game` をキャプチャして、呼び出し時点で `game.healthBar` を確認するようにします。
**教訓:** 呼び出し履歴が見せてくれるのは **今どうやってここに来たか** だけです。「その値が **いつ** 保存されたか」は、ブレークポイントを置いて再実行して探します。

## Lab 8 — 引数の順序が入れ替わっている
```
止まった関数: [外部コード] std::out_of_range を投げる ← std::array::at → statsFor(level = 200)  (テーブルは0~50)
値の出どころ: createMonster(level = 200) ← spawnMonster(name = "Ogre Boss", level = 200, bonusHp = 13)
          ← spawnWave(wave = 3)
本当の原因:   spawnMonster("Ogre Boss", 200, 10 + wave)
           → 意図はレベル 10 + wave、ボーナス HP 200
```
**修正:** `spawnMonster("Ogre Boss", 10 + wave, 200);`
**教訓:** 同じ型(int, int)の引数が並んでいると、順序を入れ替えて渡しやすくなります。強い型(`struct Level { int value; };`)や指示付き初期化で防げます。

---

# 第2部: 応用

## Lab 9 — off-by-one
`i <= scores.size()` → 最後の反復で `scores.at(5)` の範囲外アクセス → `std::out_of_range`。
**修正:** `i < scores.size()`、または範囲ベース for / `std::accumulate`。
**教訓:** 例外(またはアサーション)で止まる → 呼び出し履歴 → 最初の自分のコード、が最も基本的な追跡ルートです。`[]` の代わりに `at()` を使えば、どのコンパイラー、どの構成でも範囲外アクセスが例外として表に出ます。

## Lab 10 — 参照の無効化(vector の再割り当て)
`add` が `items_.back()` の参照を返しますが、その後の `push_back` で capacity が増えると要素が新しいメモリに移され、`sword` は解放されたメモリを指します(解放後の使用 (use-after-free))。
**修正:** 参照を長く持たずに名前/インデックスで探し直す、または `reserve`、または `std::deque`/`std::list` のようにアドレスが安定したコンテナー。
**教訓:** 「参照/ポインター/イテレーターを持っている間はコンテナーを変更しないこと。」

## Lab 11 — 一時オブジェクトを指す string_view
`makeTitle` が作った `std::string` は `titleOf` の return 文が終わると破棄され、`string_view` はその解放されたバッファーを指します。
**修正:** `std::string` で返します。`string_view` は所有者のほうが長く生きているときだけ使います。

## Lab 12 — 初期化されていないメンバー
`int combo_;` に初期値がありません。カウンターはオブジェクトプール(`bughunt::recycledStorage`)の再利用メモリの上に作られ、そこには以前使われていた値(BugHunt では `0xA5A5A5A5` = -1515870811)が残っているので、最初の4連続命中が負の数として記録されて無視されます。普通のローカル変数だったら、コンパイラーや構成によってはたまたま0になり、正しく見えることもあります。
**修正:** `int combo_ = 0;`
**教訓:** コンストラクターがすべてのメンバーを初期化しているか確認します。値が実行ごと、構成ごとに違うなら、未初期化変数を真っ先に疑います。(コンパイラーの「メンバーを初期化していない」静的解析警告。例: MSVC コード分析 C26495、clang-tidy `cppcoreguidelines-pro-type-member-init`)

## Lab 13 — erase 後のイテレーター無効化
`erase(it)` の後 `it` は無効なのに `++it` しています。
**修正:**
```cpp
for (auto it = enemies.begin(); it != enemies.end(); )
    it = (it->hp <= 0) ? enemies.erase(it) : it + 1;
// 一覧が std::vector なら、C++20 の std::erase_if(enemies, [](const Enemy& e) { return e.hp <= 0; }); で1行で書けます。
```

## Lab 14 — unsigned のアンダーフロー
`20u - 22u` = 4294967294。「残りのスロット」がとても大きくなり、何でも入ってしまいます。
**修正:** `if (bag.usedSlots >= bag.maxSlots) return false;` を先に検査するか、符号付きの型で計算します。

## Lab 15 — Rule of Three 違反(二重解放)
`PacketBuffer` は生ポインターを所有していますが、コピーコンストラクター/代入はデフォルト(浅いコピー)です。デストラクターを自分で宣言しているのでムーブコンストラクターは自動生成されず、`push_back(PacketBuffer(64))` で **コピー** が起きる → 一時オブジェクトの破棄時に `data_` を解放 → vector の中のオブジェクトは解放されたバッファーを指す → **2回目の `push_back`** で vector が再割り当てして古い要素を破棄するとき、同じバッファーをもう一度解放(double free)。その場でメモリチェッカーが `[CRASH] メモリ検査: 解放済みのメモリをもう一度解放した` で止めるので、`expect` 行まで届きません。
**修正(推奨、Rule of Zero):** `std::vector<char> data_;` または `std::unique_ptr<char[]>` を使います。または、コピーコンストラクター/代入/ムーブを自分で実装します。

## Lab 16 — 仮想デストラクターの欠落(メモリリーク + UB)
`StatusEffect` のデストラクターが virtual ではないので、`unique_ptr<StatusEffect>` が `delete` するときに `Burn` のデストラクターが呼ばれません → メンバー `schedule_`(`DamageSchedule`)も破棄されず、戦闘1000回の後に1000個が生き残ります(その中の vector のメモリもリーク)。厳密には未定義動作です。
**追跡:** `DamageSchedule` のデストラクターにブレークポイントを置くと、一度も止まりません。コンストラクターでは止まります。→ 誰が消すべきだったのか? `unique_ptr` の `delete` は `StatusEffect` のデストラクターしか呼びません。
**修正:** `virtual ~StatusEffect() = default;`
**教訓:** ポリモーフィックに delete される基底クラスには必ず virtual デストラクターを付けます。(警告: MSVC C5205(標準ヘッダー内のテンプレートなので `/external:templates-` があるときに見えます。このリポジトリの CMake は有効にしています)、Clang `-Wall` の `-Wdelete-abstract-non-virtual-dtor`。GCC はシステムヘッダーの中なので見えないことがあります)

## Lab 17 — データ競合
`gold_` の読み取り-変更-書き込みがアトミックではありません。2つのスレッドが同じ値を読んでそれぞれ +1 を書き込むと、1つが消えます。読み取りと書き込みの間の `std::this_thread::sleep_for(1ms)` はその隙間を広げて、どのコンパイラーでも症状が出るようにしているだけで、原因ではありません(消してもデータ競合は残ります)。
**修正:** `std::atomic<int> gold_{0};` + `gold_ += amount;`、または `std::mutex` で保護します。

## Lab 18 — 構造体内部のバッファーオーバーフロー
`char name[8]` に `"Sir Kael"`(8文字 + NUL = 9バイト)を `memcpy` → すぐ後ろの `hp` の先頭バイトを0で上書き → 100(0x64)が0になる。`"Sir Doran"` は10バイトなので、`hp` の下位2バイトを `'n'`、`0` で上書きして 120 → 110。
**修正:** `name` を `std::string` に変えるか、長さを制限してコピーします(`std::snprintf(c.name, sizeof(c.name), "%s", newName)`)。
**教訓:** 「誰がこの値を変えた?」はデータ ブレークポイントですぐに答えが出ます。

## Lab 19 — 循環データによる無限再帰(スタックオーバーフロー)
テーブルに `village_intro: forge_sword` の打ち間違いがあり、`forge_sword → meet_blacksmith → village_intro → forge_sword ...` と循環します。
**修正:** データを `village_intro:` に直し、コードでも防御します: 訪問済みの集合(`std::set<std::string> visited`)で循環の検出と重複の除去。
**教訓:** 呼び出し履歴が同じパターンで繰り返されていたら、再帰の終了条件/データの循環を疑います。

## Lab 20 — 演算子の優先順位
`==` が `&` より先に計算されて `flags & (status == status)` = `flags & 1` → 常に STUN ビットだけを検査します。
**修正:** `return (flags & status) == status;`
**教訓:** コンパイラーの警告(MSVC C4554、GCC・Clang `-Wparentheses`)がすでに教えてくれていました。警告は0個に保ちましょう。

---

# 第3部: フロー追跡 (クラッシュなし)

## Lab 21 — ループの中で初期化される累積変数
```
間違った結果:   最長連続1、ゴールド900(ボーナスなし)
最初にずれた行: 2周目の int streak = 0;  (1だった値が0に戻る)
本当の原因:     streak が for の中で宣言されていて、毎周新しく作られる
```
**修正:** `int streak = 0;` を `for` の上に移します。(欠席の処理 `streak = 0;` はそのまま残します。)
**教訓:** クラッシュがなければ呼び出し履歴は役に立ちません。**期待値を先に書き**、1行ずつ実行して最初にずれる行を探します。

## Lab 22 — 計算した値ではなく元の値を返す
```
段階ごとの値:   期待 70 → 105 → 84 → 126 / 実際 70 → 70 → 56 → 84
最初にずれた所: applyCrit の戻り値(関数から出た直後にデバッガーが見せる戻り値: "applyCrit が 70 を返した")
本当の原因:     result に元の値をコピーしておき、damage だけ変えてから result を返している
```
**修正:** `return damage;`(または `return isCrit ? damage * (100 + critBonusPercent) / 100 : damage;` で一時変数をなくす)
**教訓:** パイプラインは段階ごとの期待値を決めておいて1行ずつ進めれば、犯人の関数がすぐに見つかります。関数の中へはステップ イン (Step Into)、出てきたら **デバッガーが見せる戻り値**(Visual Studio の自動変数 ウィンドウ、gdb `finish`)。

## Lab 23 — シールドが壊れるときに残りのダメージが消える
```
間違った結果:   受けたダメージ160、シールド吸収70 + HP減少87 = 157(3が消えた)
止めた位置:     条件付きブレークポイント (before.shield - tank.shield) + (before.hp - tank.hp) != damage
最初にずれた所: シールドが damage より少なく残っていたティック(例: シールド6にダメージ7)
本当の原因:     takeDamage がシールドを0まで削ってすぐ return → あふれたダメージが HP に行かない
```
**修正:**
```cpp
const int absorbed = std::min(unit.shield, damage);
unit.shield -= absorbed;
unit.hp -= damage - absorbed;
```
**教訓:** 数百回のうち数回だけ間違うなら、1行ずつ実行しても見つかりません。「間違った瞬間」を **条件式** で表して、ブレークポイントに入れます。合計の検証(不変条件)は、その条件を作る良い材料です。

## Lab 24 — イベントの購読順序
```
トレースポイントの出力(5匹目のオオカミ):  クエスト検査: カウンター値 4  →  カウンター: Wolf、増加前 4
本当の原因:     Game::setup でクエストをカウンターより先に購読 → クエストが更新前の値を読む
```
**修正:** カウンターを先に購読します。もっと堅い方法は、クエストがほかの購読者の状態に頼らず自分で数えるか、カウンターが「更新後」のイベントを別に出すことです。
**教訓:** 「どちらが先に実行されるか」という問題は、止めるとかえって混乱します。**トレースポイント** で止めずに順序を記録しましょう。

## Lab 25 — else if の順序(広い条件が狭い条件を隠す)
```
ウォッチ (Selene):  c.isFirstOrder() false / c.isMember() true / c.isVip() true
矢印:            else if (c.isMember()) → return 5
本当の原因:       VIP は常にメンバーなので、isVip() の分岐には永遠に届かない
```
**修正:** 狭い条件(VIP)を広い条件(メンバー)より先に検査します。ルールが「最も大きい割引を1つ」なので、順序に依存しないように `std::max` で該当する割引の中の最大値を選ぶほうがより安全です。
**教訓:** 分岐のバグは、ウォッチに **条件を1つずつ別々に入れて計算** してみると一目でわかります。

## Lab 26 — auto が参照を捨ててコピーを作る
```
ウォッチ:    &item != &backpack.get("Iron Sword")
本当の原因:  auto item = backpack.get(...)  → get() は Equipment& を返すが、auto は値で受け取ってコピーする
             コピーを修理し、コピーで「修理完了」を出力している
```
**修正:** `auto& item = backpack.get(itemName);`
**教訓:** 「直したと出力されるのに反映されない」なら、**同じオブジェクトかどうかから** 疑いましょう。アドレスの比較やオブジェクト ID で確認します。

## Lab 27 — || の短絡評価で後ろの検査が実行されない
```
ブレークポイント: checkKill で止まる、checkGold・checkLevel で止まらない
本当の原因:   checkKillAchievements(p) が true → || の後ろは計算しない → ベテラン実績の検査そのものが実行されない
```
**修正:** 副作用のある呼び出しは、それぞれ実行してから結果を合わせます。
```cpp
const bool kill  = checkKillAchievements(p);
const bool gold  = checkGoldAchievements(p);
const bool level = checkLevelAchievements(p);
if (kill || gold || level) ...
```
(`|` に変えても動きますが、意図が隠れるのでお勧めしません。)
**教訓:** **止まらなかったブレークポイント** も結果です。「この関数はそもそも呼ばれたのか?」から確認しましょう。

## Lab 28 — 設定を読む前にコピーしておいたメンバー
```
ウォッチ (boot の中、loadSettings の直後):
    g_settings.difficulty         Hard
    world_.settings_.difficulty   Normal
本当の原因:  World は Settings を値で保持し、Game のコンストラクターで(= loadSettings の前に)コピーされる。
             AudioSystem は参照で保持するので、後から読んだ音量が反映される。
```
**修正(どれか1つ):**
- `World` も `const Settings& settings_;` で保持する(設定の変更がすぐ反映される、ただし寿命の管理に注意)
- 設定をすべて読んだ後で `World` を作る(初期化の順序を明確に)
- `spawnWave(const Settings&)` のように、必要なときに引数で受け取る

**教訓:** バグが「どんな入力で」ではなく「どんな **順序** で」起きる場合は、最初から追いかけながら **2つのコピーを並べて** ウォッチに表示し、いつ分かれるかを見ます。

## 演算子のミス(ラボ29~32)


## Lab 29 — 後置インクリメントの値は増やす前の値
```
間違った結果:   セボムが1007番(ドユンと同じ番号)。lastIdは1010で合っているが、実際に渡した番号は1007~1009
止めた位置:     issueId の return 行、最初の join
期待値 → 実際:  lastId 1007 → 新しい番号1008を期待 / 実際の戻り値1007
本当の原因:     return guild.lastId++; は増やす前の値(最後にすでに渡した番号)を返す
```
**修正:** `return ++guild.lastId;`
**教訓:** `x++` は「xを増やす」と「式の値は古いx」の2つのことをします。変数の意味(「最後に渡した番号」か「次に渡す番号」か)を先に決めてから前置/後置を選びます。`if (hits++ == 3)` も同じミスで、4回目の攻撃でやっと真になります。

## Lab 30 — マクロが副作用のある引数を2回評価する
```
間違った結果:   画面のコンボ 2 4 6 0 1 2(期待 1 2 3 0 1 2)、最高コンボ6
止めた位置:     c.current のデータ ブレークポイント → 攻撃1回で2回止まる
最初にずれた行: c.best = BUGHUNT_MAX(c.best, ++c.current);
本当の原因:     ((a) > (b) ? (a) : (b)) に展開され、条件で1回、選ばれた側でもう1回 ++c.current
```
**修正:**
```cpp
++c.current;
c.best = std::max(c.best, c.current);
```
**教訓:** マクロは関数ではなく文字の置き換えです。引数に `++`、`--`、関数呼び出しを入れないでください。できれば `std::max` や `inline`/`constexpr` 関数を使います。(`?:` は条件を先に計算し、選ばれた側だけを計算する順序が決まっているので、この結果はどのコンパイラーでも同じです。)

## Lab 31 — `&&` の代わりにビットAND `&`
```
間違った結果:   ボラム(鍵2、たいまつ1)入場不可、入場人数1
止めた位置:     canEnterCave、条件 a.name == "보람"(ボラム)
期待値 → 実際:  a.keys & a.torches → 真を期待 / 実際は0(2 = 10、1 = 01)
本当の原因:     整数2つを & で結ぶと、共通のビットがなければ0(偽)になる
```
**修正:** `return a.keys > 0 && a.torches > 0;`
**教訓:** `&` は整数のビット同士、`&&` は真偽同士を計算します。両方1のときだけ偶然同じ答えになるので、最初のテスト(1, 1)は通ってしまいます。個数を真偽として使うときは、`> 0` で意味を書いておきます。

## Lab 32 — `||` の代わりに `|` → 短絡評価がなくなる
```
間違った結果:   開いた扉4(期待5)、残りのロックピック0(期待1)
止めた位置:     useLockpick の --t.lockpicks 行、呼び出し履歴で openDoor の door を確認
最初にずれた所: 「牢屋」の扉(鍵があるのにロックピックが減る)
本当の原因:     hasKey(...) | useLockpick(...) は左が true でも右を呼ぶ
```
**修正:** `return hasKey(t, door) || useLockpick(t);`
**教訓:** 短絡評価をするのは `||`/`&&` だけです。右側のオペランドに副作用(消費、セーブ、ログ)があると、`|`/`&` はその副作用を必ず起こします。ラボ27と対になります: 27は短絡評価のせいで **必要な呼び出しが抜け**、32は短絡評価がないせいで **不要な呼び出しが起きました**。`p == nullptr | p->hp <= 0` のように左の検査が右を守っている式では、`|` はnull参照のクラッシュになります。

---

# 第4部: コンテナー

## Lab 33 — map::operator[] が存在しないキーを作る
```
観察:        openShop の前は bag.size() == 3、後は == 6("Mana Potion"、"Fire Scroll"、"Rope" が個数0で生まれる)
本当の原因:  countOf の bag[item] はキーがなければ既定値(0)で要素を新しく作る
```
**修正:** 読むだけのときは `find`(または C++20 の `contains`)を使い、`const Bag&` で受け取ります。
```cpp
int countOf(const Bag& bag, const std::string& item)
{
    const auto it = bag.find(item);
    return it == bag.end() ? 0 : it->second;
}
```
`const Bag&` に変えると `bag[item]` はコンパイルエラーになり、同じミスを防いでくれます。
**教訓:** `operator[]` は「検索」ではなく「検索して、なければ作る」です。

## Lab 34 — rehash の後で「何行目」が別の人を指す
```
ウォッチ:    registerWave(..., 60) の前後で bucket_count が大きく増え、selectedName() が Mira → 別の人に変わる
本当の原因:  選択を巡回順の位置(行番号)で覚えている。rehash が要素を新しいバケットに振り分け直すので順序が変わる
```
**修正:** rehash でも変わらないもので覚えます。規格は、rehash がイテレーターを無効化しても **要素への参照とポインターは保たれる** と保証しています。
```cpp
void select(size_t row) { selected_ = &*std::next(board_.begin(), row); }
const std::string& selectedName() const { return selected_->first; }
std::string confirmInvite()
{
    auto it = board_.find(selected_->first);
    ...
}
Board::value_type* selected_ = nullptr;
```
キー(名前)をコピーしておいて `find` してもかまいません。
**教訓:** `unordered_map` の巡回順は約束ではありません。長く持っておくものはキーか要素へのポインターで。

## Lab 35 — 比較関数が strict weak ordering に違反する
```
止めた場所:  [CRASH] ソート検査: 比較関数が狭義の弱順序(strict weak ordering)を破った ← bughunt::checkedOrder ← std::sort
ウォッチ:    comp(Kael, Doran) == true、comp(Doran, Kael) == true (どちらも1500点)
本当の原因:  >= は等しい値に対して true を返す。「a が b より前で、b も a より前」という矛盾
```
**修正:** `return a.score > b.score;`
**構成ごと:** ラボでは比較関数を `bughunt::checkedOrder` で包み、どのコンパイラー、どの構成でも同じ場所で止まるようにしました。このチェックがなければ標準ライブラリごとに異なります。MSVC の Debug STL は `invalid comparator` アサーションを出し、libstdc++ は `-D_GLIBCXX_DEBUG`、libc++ はハードニングモードでのみチェックします。チェックのない Release では、要素が少なければ偶然正しくソートされ、等しい値がとても多いとソートが配列の外まで読み進んで落ちることもあります。
**教訓:** 比較関数は「厳密に前にあるか」に答えます。等しければ false です。

## Lab 36 — remove_if だけして erase を忘れる
```
観察:        呼び出し後も size は 6。前の3つは残す装備、後ろの3つは移動された後の残りかす(remove_if は要素を **移動** するので、名前は空で耐久度だけが残る)
警告:        戻り値を捨てている(MSVC C4858、GCC・Clang -Wunused-result。remove_if は [[nodiscard]])
```
**修正:** C++20 の `std::erase_if(bag, pred);`、または erase-remove イディオム `bag.erase(std::remove_if(bag.begin(), bag.end(), pred), bag.end());`
**教訓:** アルゴリズムはイテレーターしか受け取らないので、コンテナーのサイズを変えられません。警告を0個に保っていれば、このバグはビルド時に見えます。

## Lab 37 — vector<bool> のプロキシ参照
```
ローカル:    wasOpen の型が bool ではなくビットプロキシ(MSVC std::_Vb_reference、libstdc++ std::_Bit_reference、libc++ std::__bit_reference)
             トグルした次の行で wasOpen も一緒に変わる
本当の原因:  vector<bool> はビットに圧縮されているので operator[] が bool& を返せず、ビットを指すプロキシオブジェクトを返す。
             auto はそのプロキシをコピーするので、「以前の値」ではなく「今の値」を指し続ける
```
**修正:** `const bool wasOpen = doorsOpen[door];`(型を書いて値として取り出す)。ビット圧縮が不要なら `std::vector<char>` や `std::vector<std::uint8_t>` も良い選択です。
**教訓:** `auto x = v[i];` がコピーだという思い込みは `vector<bool>` で崩れます。デバッガーで変数の型を見ましょう(gdb `ptype`、lldb `frame variable -T`)。

## Lab 38 — c_str() ポインターの寿命と SSO
```
ウォッチ:    (void*)heroes[i].name.c_str() が grantTitle の後で変わる(Doran、Selene)、Kael はそのまま
Kael:        "Kael the Brave" 14文字 → まだオブジェクト内のバッファー(SSO) → 同じアドレスなので偶然合う
Doran:       21文字 → オブジェクト内のバッファーからヒープへ引っ越し。ネームプレートはオブジェクト内の古いバッファー(今はヒープへのポインターが入っている)を読んで文字化け
Selene:      もともとヒープ(18文字、MSVC STL では容量31) → 37文字で再割り当て。ネームプレートは解放されたヒープを読む(メモリチェッカーの 0xEE の文字)
```
**修正:** ネームプレートが文字列バッファーではなく、文字列を持つオブジェクトを指すようにします。
```cpp
explicit Nameplate(const Hero& hero) : hero_(&hero) {}
const char* text() const { return hero_->name.c_str(); }
const Hero* hero_;
```
**構成ごと:** 上の文字数の境界は MSVC STL と libstdc++(オブジェクト内バッファー15文字)が基準です。ヒープの容量も実装ごとに違います(18文字のとき MSVC STL は31、libstdc++ は18)。libc++(macOS の既定)は22文字までオブジェクト内に置くので、Doran も偶然合うことがあり、Selene も最初はオブジェクト内にあって Doran と同じようにヒープへ移ります。ASan が捕まえるのは Selene だけです(heap-use-after-free、MSVC STL・libstdc++ 基準)。Doran はまだ生きている `Hero` オブジェクト内のメモリを読むので、ASan から見れば正常なアクセスです。
**教訓:** `c_str()`/`data()`/`string_view` は文字列が変わると無効になることがあります。短い文字列は SSO のせいで「動いているように」見えるので、発見がさらに遅れます。

## Lab 39 — set の中にある要素のソート基準を変える
```
ウォッチ:    season.ranked_ の順序が Mira(1450)、Selene、Kael、Doran(1700) → 点数順ではない
本当の原因:  recordMatch が set に入っている Player の score を直接変える。set は挿入するときにしか位置を決めないので、木がソートの不変条件を失う。
             その後の find/erase は見当違いの枝へ下りていき、要素を見つけられないことがある
```
**修正:** 取り出して、変えて、入れ直します。
```cpp
void recordMatch(Player* p, int points)
{
    ranked_.erase(p);
    p->score += points;
    ranked_.insert(p);
}
```
(C++17 の `extract` でノードを取り出して直してから `insert` してもかまいません。)
**教訓:** `set`/`map` のキーは const ですが、ポインターをキーにすると、指している先までは守ってくれません。

---

# 第5部: C++ 言語の落とし穴


## Lab 40 — `const` への `std::move` はコピー、移動後の元に頼る
```
間違った結果:   自分のトレード画面に3個がそのまま(期待0)、コピー3回(期待0)
止めた位置:     Item のコピーコンストラクターのブレークポイント → 呼び出し履歴の1つ上、trade の push_back(const Item&)
本当の原因:     from が const Trader& なので item は const Item&。std::move(item) は const Item&& になりコピーコンストラクターへ行く。
                さらに、移動できても要素を移動しただけで vector から消してはいない
```
**修正:**
```cpp
void trade(Trader& from, Trader& to)
{
    for (Item& item : from.offer)
        to.received.push_back(std::move(item));
    from.offer.clear();
}
```
**教訓:** `std::move` はキャストにすぎず、何も移動しません。`const` オブジェクトに使うと黙ってコピーになります。移動されたオブジェクトは「有効だが未規定の状態」なので、代入するか空にすることだけが安全で、サイズや中身に頼ってはいけません(use-after-move)。静的解析ツールには、この二つのミスをどちらも見つけるチェックがあります。

## Lab 41 — 符号付き整数のオーバーフロー、オーバーフロー後のチェック
```
間違った結果:   経験値 -2144967296(期待 2000000000)
止めた位置:     addExp の if の行、条件付きブレークポイント total < 0。current 1950000000、baseExp * multiplier 2000000000000
本当の原因:     int の足し算がオーバーフローして負の数になり、上限チェックはすでに壊れた値を見ている
```
**修正:**
```cpp
long long total = static_cast<long long>(current) + static_cast<long long>(baseExp) * multiplier;
if (total > kMaxExp)
    total = kMaxExp;
return static_cast<int>(total);
```
**教訓:** 符号付き整数のオーバーフローは未定義動作です。たいていは負の数に回り込みますが、コンパイラーは「オーバーフローしない」と仮定してよいので、`if (a + b < a)` のような「オーバーフローしたかを見る」チェックは Release で丸ごと消えることがあります(このリポジトリのラボはそうしたチェックを使わないので、構成ごとに結果は同じです)。チェックはオーバーフローする **前に** 行います。より広い型で計算するか、`std::numeric_limits<int>::max() - current < gain` のように残りの余裕を見ます。オーバーフローした後で `total < 0` を見るつぎはぎは、掛け算があふれて正の数に戻る場合(隠しテスト)を見逃します。

## Lab 42 — 縮小変換が二か所(`double`→`int`、`int`→`short`)
```
間違った結果:   ポーション1個の銀貨 114(期待115)、300個まとめ売り -31336(期待34500)
止めた位置:     toSilver の return の行、ウォッチ gold * 100 = 114.99999999999999 / sellBundle で toSilver(unitGold) * count と record.totalSilver
本当の原因:     1.15 は2進数で正確に表せず 1.1499... として保存され、double→int は切り捨て。総額のフィールドが short なので 34200 が切り落とされる
```
**修正:** `return static_cast<int>(std::lround(gold * 100));`、`SaleRecord::totalSilver` を `int` に
**教訓:** `=` や `return` の縮小変換は警告だけか(MSVC C4244)、何も言われません。波かっこ初期化(`int silver{ gold * 100 };`)は縮小変換をコンパイルエラーにして(MSVC・Clang。GCC は定数でない値には警告だけなので `-Werror=narrowing`)、二か所を一度に見つけてくれます。お金は最初から最小単位の整数で持ち歩きます。`+ 1` で合わせるつぎはぎは 2.5 ゴールド(ちょうど250)で間違えます。

## Lab 43 — `std::min<std::size_t>` が負の数をとても大きな正の数に
```
間違った結果:   弓使いジン(平均より12高い)のボーナス20(期待0)
止めた位置:     levelBonus、条件付きブレークポイント myLevel > average。average - myLevel = -12、size_t では 18446744073709551604
本当の原因:     コンパイルエラーを避けるため std::min の型を std::size_t に決めたので、負の数が符号なしに変換された
```
**修正:** `return std::clamp(average - myLevel, 0, static_cast<int>(maxBonus));`
**教訓:** 符号付きと符号なしの値が出会うと、符号付きの方が符号なしの型に変わります(`-1 < v.size()` が false になるのと同じ規則)。型の不一致によるコンパイルエラーをキャストで黙らせると、エラーが知らせていた問題が隠れます。混ぜて比較する必要があるなら C++20 の `std::cmp_less` などを使います。

## Lab 44 — 二つのファイルのグローバル変数が互いを使う(初期化順序)
```
間違った結果:   戦士のHP 0(期待300)。ファイル順を変えてビルドするとボスの攻撃力が0
止めた位置:     g_warriorHp の初期化の行のブレークポイント、呼び出し履歴に main がない。そのとき g_baseHp == 0
本当の原因:     翻訳単位の間のグローバル初期化の順序は決まっていない(普通はリンク順)。二つのファイルが互いの値を使うので、どちらが先でも一方は0を読む
```
**修正:** グローバル変数八つを関数ローカルの static に。
```cpp
int baseAttack() { static const int value = readStat("base_attack", 12); return value; }
int bossAttack() { static const int value = baseAttack() * 4; return value; }
// 残りの六つも同じ形
```
**教訓:** 別のファイルのグローバルオブジェクトに頼るグローバル初期化は、順序任せの賭けです。関数の中の static は最初に呼ばれたときに作られ(C++11 からスレッドセーフ)、依存する側が呼んだ瞬間に用意されます。コンパイル時に決められる値なら `constexpr`/`constinit` が最善です。このラボは両方向に依存させているので、リンク順が変わってもバグのある状態では常に FAIL です。画面の二つの値だけを定数で埋め込むつぎはぎは、隠しテストの別の二つの値で引っかかります。

## Lab 45 — ヘッダーの `static` 変数がファイルごとに別々
```
間違った結果:   セーブ回数0、最後のスロット ""(期待3、"auto-竜の巣")
止めた位置:     lg06_autosave.cpp の ++g_saveStats.count と showSaveScreen で &g_saveStats が異なる
本当の原因:     名前空間スコープの static は内部リンケージ。ヘッダーをインクルードした .cpp ごとに別の g_saveStats ができる
```
**修正:** ヘッダーで `inline SaveStats g_saveStats;`(C++17 の inline 変数)。またはヘッダーに `extern` 宣言、.cpp 一か所で定義。
**教訓:** ヘッダーに定義を置くときの `static` は「一つだけ」ではなく「ファイルごとに一つずつ」です。コンパイラーもリンカーも警告しません。ヘッダーの `const`/`constexpr` 変数も既定で内部リンケージなので、値は同じでもアドレスは異なります。

## Lab 46 — 例外の経路でだけ漏れるテクスチャー
```
間違った結果:   残ったテクスチャー2個(期待0)
止めた位置:     Texture のコンストラクター/デストラクターのブレークポイント: 生成6回、破棄4回。残った二つは「呪われた城」
本当の原因:     loadDungeon がテクスチャーを new した後で countRooms が投げると、生ポインター floor/walls だけが消えて Texture は残る
```
**修正:**
```cpp
auto floor = std::make_unique<Texture>(file.name + "/floor");
auto walls = std::make_unique<Texture>(file.name + "/walls");
const int rooms = countRooms(file.layout);
return new Dungeon{ file.name, floor.release(), walls.release(), rooms };
// さらに良いのは Dungeon のメンバーも unique_ptr にし、loadDungeon は std::unique_ptr<Dungeon> を返すこと
```
**教訓:** 例外が起きるとその間の関数の残りの行は実行されず、デストラクターが呼ばれるのはスタックのオブジェクトだけです。資源は作ったらすぐ所有オブジェクト(RAII)に任せます。正常な経路だけを見ると new/delete の対が合っているように見えるので、見つけにくいリークです。

## Lab 47 — デストラクターから投げて `std::terminate`
```
間違った結果:   CRASH、abort()(セーブ失敗のメッセージが出ない)
止めた位置:     例外が投げられた ~SaveFile。呼び出し履歴の下に saveGame の catch ではなく std::terminate
本当の原因:     C++11 からデストラクターは暗黙の noexcept。noexcept の関数の外へ出ようとする例外は catch を探さず terminate
```
**修正:** 失敗しうる書き込みを `close()` に出します。
```cpp
void close()
{
    if (buffer_.size() > freeBytes_)
        throw SaveError("디스크 공간이 모자란다");   // ディスク容量が足りない
    g_disk[slot_] = buffer_;
    closed_ = true;
}
~SaveFile()
{
    if (!closed_)
    {
        try { close(); } catch (...) {}
    }
}
// saveGame: try ブロックの最後の行で file.close();
```
**教訓:** デストラクターは失敗を知らせる手段がない場所です。失敗しうる後始末は明示的な関数(`close`、`commit`)に出し、デストラクターでは例外を握りつぶします。`noexcept(false)` にするつぎはぎは、別の例外でスタックを巻き戻している最中にデストラクターがさらに投げると、やはり terminate です(隠しテスト)。MSVC は C4297、clang は `-Wexceptions`、GCC は `-Wterminate` ですでに警告しています。関数に `noexcept` を付けておいて中で投げる場合も同じ結果です。

## Lab 48 — 範囲 for は範囲を最初に一度だけ評価する
```
間違った結果:   今回のターンの完了1(期待3)、日誌3個(期待5)
止めた位置:     endTurn の for。ループは2回、その間に log.size() は3に増えた。&q は日誌の中ではなく entries() が返したコピーの中
本当の原因:     範囲 for は log.entries() を一度評価し、その begin/end を取っておく。ループ中に日誌に追加されたクエストはコピーにも、取っておいた終端の内側にもない
```
**修正:**
```cpp
for (std::size_t i = 0; i < log.size(); ++i)
{
    const Quest q = log.at(i);   // コピーで受け取る(complete 内の push_back が再割り当てしても安全)
    if (q.done || achieved.count(q.goal) == 0)
        continue;
    log.complete(q.name, book);
    ++completed;
}
```
**教訓:** 範囲 for は `auto&& r = 範囲式; auto b = r.begin(); auto e = r.end();` に展開されます。ループ中に大きくなるコンテナーはインデックスで回し、毎回 `size()` を読み直します。`entries()` が参照を返すように変えるのは答えではありません。ループ中の vector に push_back すると終端イテレーターが無効になり(未定義動作)、再割り当てが起きると `q` も解放されたメモリを指します(ラボ10)。

> 設計メモ: 最初は `vector` を直接範囲 for で回し、`reserve` しておいてから push_back するラボとして作りました。しかしこれは終端イテレーターの無効化(未定義動作)なので、同じ MSVC STL の Debug でも MSVC でビルドすると FAIL、clang++ でビルドすると "vector iterators incompatible" アサーションで CRASH になりました。判定がコンパイラーごとに異なるため、今のように「コピーを回す」定義された形に変え、無効化はヒントと教訓に残しました。

---

# 第6部: インターフェースと継承


## Lab 49 — `const` が抜けてオーバーライドではなく隠蔽
```
間違った結果:   背後からのバックスタブ20(期待60)
止めた位置:     Backstab::damage のブレークポイントは止まらない → useSkill からステップ イン (Step Into) → Skill::damage
本当の原因:     Backstab::damage に const がなく、Skill::damage(const Target&) const とは別の関数。Skill& で呼ぶと基底版
```
**修正:** `int damage(const Target& t) const override { ... }`
**教訓:** オーバーライドするつもりの関数には必ず `override`。付けた瞬間、`const` 漏れ、引数の型の違い(`int` と `long`、`const&` と値)、基底側の名前変更をコンパイラーがエラーで教えてくれます。clang と GCC 13 以降は `-Wall` で `-Woverloaded-virtual` 警告を出しますが、MSVC は既定の警告レベル(/W3)では何も言いません。

## Lab 50 — `vector<Monster>` に入れて Boss が切られる(スライシング)
```
間違った結果:   戦利品3個、黄金の王冠なし(期待4個)
止めた位置:     Boss::loot のブレークポイントは止まらない → collectLoot で wave[2] を展開すると型は Monster、rare_ がない
本当の原因:     wave.push_back(Boss(...)) が Boss の Monster 部分だけをコピーして格納する
```
**修正:**
```cpp
std::vector<std::unique_ptr<Monster>> spawnWave(const std::vector<SpawnInfo>& infos)
{
    std::vector<std::unique_ptr<Monster>> wave;
    for (const SpawnInfo& info : infos)
    {
        if (info.rareDrop.empty())
            wave.push_back(std::make_unique<Monster>(info.name, info.hp));
        else
            wave.push_back(std::make_unique<Boss>(info.name, info.hp, info.rareDrop));
    }
    return wave;
}
// collectLoot も同じ型を受け取って m->loot()
```
**教訓:** ポリモーフィックなオブジェクトはポインター(所有は `unique_ptr`)で格納します。値で受け取る関数引数(`void f(Monster m)`)も同じように切り落とします。ポリモーフィックな基底クラスのコピーを `= delete` で禁止しておくと、このミスはコンパイルエラーになります。

## Lab 51 — コンストラクター内の仮想呼び出し
```
間違った結果:   スナイパータワーの射程3(期待8)、距離7を撃てない
止めた位置:     Tower::baseRange のブレークポイント → 呼び出し履歴に Tower::Tower ← SniperTower::SniperTower
本当の原因:     基底のコンストラクターが動いている間、オブジェクトの動的な型はまだ Tower。仮想呼び出しも Tower 版に行く
```
**修正:** 射程を必要なときに問い合わせます。
```cpp
int range() const { return baseRange(); }
bool canHit(int distance) const { return distance <= range(); }
```
(または `Tower(int range)` コンストラクターで派生クラスが値を渡す)
**教訓:** コンストラクターとデストラクターの中では、「今作っている(壊している)クラス」の仮想関数が呼ばれます。これは規格で決まった動作なので、どのコンパイラーでも同じです。デストラクターから **純粋** 仮想関数を呼ぶ場合は未定義動作なので、このラボからは外しました(下の未収録を参照)。

## Lab 52 — 仮想関数のデフォルト引数は静的な型に従う
```
間違った結果:   プリーストの基本回復15(期待30)、パーティー合計25(期待40)。直接 priest.heal() は30
止めた位置:     Priest::heal の中、castDefaultHeal から入ったとき amount == 10
本当の原因:     どの heal が呼ばれるかは動的な型(Priest)で、デフォルト引数は静的な型(Healer)で決まる
```
**修正:** 既定値も仮想関数で問い合わせます。
```cpp
// Healer
virtual int defaultAmount() const { return 10; }
// Priest: return 25;  Druid: return 18;
int castDefaultHeal(const Healer& healer)
{
    return healer.heal(healer.defaultAmount());
}
```
**教訓:** 仮想関数にデフォルト引数を付けません(付けるとしても基底と同じ値だけ)。よくある定石は、既定値を使う非仮想関数が仮想関数を呼ぶ NVI(non-virtual interface)の形です。基底の既定値だけを25に変えると、ドルイド(18)で間違えます(隠しテスト)。

## Lab 53 — 派生クラスのオーバーロード1つが基底のオーバーロードをすべて隠す
```
間違った結果:   ゴールド0(期待500)、アイテム2個(期待1)。アイテム一覧に500番
止めた位置:     box.give(500) からステップ イン → Item::Item(int) に入る
本当の原因:     EventMailbox が give(const Item&) を宣言したため Mailbox::give(int) が隠れた。500 は explicit のない Item(int) で変換
```
**修正:** `EventMailbox` に `using Mailbox::give;`(そして `Item` のコンストラクターに `explicit`)
**教訓:** 名前探索はオーバーロード解決より先です。派生クラスで同じ名前を1つでも宣言すると、基底の同じ名前はすべて見えなくなります。見える候補が1つだけで暗黙の変換ができると、エラーなしで見当違いの関数が呼ばれます。数値1つから作るコンストラクターには `explicit` を。

## Lab 54 — `dynamic_cast` の結果の nullptr 確認漏れ
```
間違った結果:   クラッシュ(アクセス違反 / SIGSEGV)
止めた位置:     Mage::restoreMana、this == nullptr → 呼び出し履歴の1つ上 drinkManaPotions: unit は Warrior、mage は nullptr
本当の原因:     ポインターに対する dynamic_cast は型が合わなければ nullptr を返す。確認せずに呼んだ
```
**修正:**
```cpp
if (Mage* mage = dynamic_cast<Mage*>(unit.get()))
{
    mage->restoreMana(amount);
    ++used;
}
```
**教訓:** `dynamic_cast<T*>` は「型が合うか尋ねる」演算です。結果を確認しないなら、尋ねる意味がありません。失敗を例外で受け取りたいなら参照版(`dynamic_cast<T&>`、失敗時は `std::bad_cast`)を使います。型による分岐が増えてきたら、仮想関数に移すべきサインです。

---

# 第7部: ファイル入出力


## Lab 55 — 相対パスが作業ディレクトリ基準で開かれる
```
間違った結果:   ポーション100(期待120)、帰還の書 -1(価格表にない)
止めた位置:     loadPrices の std::ifstream 行
期待値 → 実際:  開くファイル game/data/prices.txt を期待 / 実際は workspace/data/prices.txt(古いコピー)
本当の原因:     "data/prices.txt" は実行ファイルのフォルダーではなく現在の作業ディレクトリ基準
```
**修正:** `std::ifstream in(gameDir() / "data/prices.txt");`
**教訓:** 作業ディレクトリはプログラムを起動した側(IDE、ショートカット、別のプログラム)が決めます。実行ファイルの隣のデータは、実行ファイルの場所を基準に絶対パスを作ります。開けなかったときに空のリストを黙って返さず、知らせます(隠しテストのようにファイルがまったくなければ品目0個になります)。

## Lab 56 — バッファーにだけあってファイルにはまだない内容を読み直す
```
間違った結果:   日誌ウィンドウ0行(期待3)
止めた位置:     readJournal を呼ぶ行で止めて journal.txt を確認 → 0バイト
本当の原因:     ofstream が開いている間に書いた内容はストリームのバッファーにあり、flush/close するまでディスクにはない
```
**修正:** `record` で書いた後に `out_.flush();`。flush は Journal の中で行います。本文の `readJournal` の前にだけ flush・close を入れると、日誌を書いてすぐ読む別の場所(隠しケース)でまた間違えます。
**教訓:** 同じプログラムの中で書いて読み直すなら、読む前に書いた内容がディスクにあることを保証します。第12部ラボ96(`std::endl` が毎回 flush して遅い)と対になります: flush は必要な瞬間にだけ。

## Lab 57 — `while (!in.eof())` で最後の記録が2回
```
間違った結果:   4行、最後に「ドユン 7100」がもう一度、合計32100(期待25000)
止めた位置:     push_back、条件 scores.size() == 3
期待値 → 実際:  4周目はないはず / 実際は in.fail() == true なのに前の値で push_back
本当の原因:     eof() は末尾を越えて読もうとして失敗した「後で」立つ。最後の改行が残っていて1周多く回る
```
**修正:** `while (in >> name >> points) scores.push_back({ name, points });`
**教訓:** 読み込みは「読む → 成功を確認 → 使う」の順番です。失敗した `>>` は変数をそのままにするので(このラボ)、前の値がもう一度入ります。`pop_back` のその場しのぎは、改行なしで終わるファイルで本物の記録を消してしまいます。

## Lab 58 — `>>` の後の `getline` が残った改行を読む
```
間違った結果:   最初のセリフ ""(空行)、3番目の位置に2番目のセリフ、最後のセリフなし
止めた位置:     push_back、1周目で line.size() == 0
本当の原因:     in >> count は「3」までしか読まず改行を残す。getline はその改行までを空行として読む
```
**修正:** `in >> count;` の後に `in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');`(`<limits>`)
**教訓:** `>>`(トークン単位)と `getline`(行単位)を混ぜるなら、境目の改行を自分で処理します。`in >> std::ws` も通ったように見えますが、次の行の先頭の空白まで食べて字下げしたセリフを壊します(隠しテスト)。

## Lab 59 — failbit が残って以降の読み込みがすべて無視される
```
間違った結果:   モンスター3種、報酬0個(期待3)、102番の報酬0
止めた位置:     in >> header 行、ウォッチ in.fail() == true
本当の原因:     最初の while が見出しの「報酬」を数値として読もうとして失敗 → failbit。消さなかったので header とゴールドの読み込みが全部素通り
```
**修正:** `std::string header;` の前に `in.clear();`
**教訓:** 失敗フラグは自然には消えません。失敗を「想定した」地点(セクションの終わり)では `clear()` で消してから読み続けます。`clear()` は文字を捨てないので、失敗を起こしたトークン(「報酬」)はそのまま次の読み込みに入ります。

## Lab 60 — UTF-8のBOMが最初のキーにくっつく
```
間違った結果:   音量80(既定値、期待30)。全画面(2行目)は正常
止めた位置:     getInt、条件 key == "volume" → settings の最初のキーは "volume" に見えるが size() == 9
本当の原因:     ファイル先頭のBOM(EF BB BF)3バイトが1行目のキーの前に付いて "\xEF\xBB\xBFvolume" になる
```
**修正:** 1行目が `"\xEF\xBB\xBF"` で始まるときだけ3バイトを消します。
```cpp
bool firstLine = true;
while (std::getline(in, line))
{
    if (firstLine && line.rfind("\xEF\xBB\xBF", 0) == 0)
        line.erase(0, 3);
    firstLine = false;
    ...
```
**教訓:** 「1行目だけがおかしい」なら、ファイル先頭の見えないバイトを疑います。デバッガーが見せる文字より `size()` とバイト値を信じます。無条件に3バイト捨てるその場しのぎは、BOMのないファイルの最初のキーを壊します(隠しテスト)。

## Lab 61 — 保存中の失敗で半分だけ書かれたセーブ
```
間違った結果:   保存失敗の後に読み込んだアイテム3種(期待5種)
止めた位置:     writeItem の throw 行、止めたまま save.txt を開く → すでに消えて前の数行だけ
本当の原因:     trunc で開いた瞬間に古いセーブが消え、例外で抜けるときに ofstream のデストラクターが前半だけのファイルを閉じる
```
**修正:** 一時ファイルに最後まで書いて閉じた後でだけ置き換えます。
```cpp
fs::path temp = file;
temp += ".tmp";
try
{
    {
        std::ofstream out(temp, std::ios::trunc);
        for (const Item& item : items)
            writeItem(out, item);
        out.close();
        if (!out)
            throw std::runtime_error("쓰기 실패");   // 書き込み失敗
    }
    fs::rename(temp, file);          // 同じフォルダーの中で置き換える
    return true;
}
catch (const std::exception& e)
{
    std::cout << i18n::L("  저장 실패: ", "  保存失敗: ") << e.what() << '\n';
    std::error_code ec;
    fs::remove(temp, ec);            // 失敗した一時ファイルを片付ける
    return false;
}
```
**教訓:** 保存は「全部か無か」でなければなりません。新しい内容は一時ファイルに、成功した後でだけ `rename`。一時ファイルは同じフォルダー(同じドライブ)に置いてこそ、rename がコピーなしの置き換えになります。rename の前にストリームを閉じます(開いているファイルは OS によっては名前を変えられません)。実際のゲームでは電源が切れる場合まで備えて、rename の前にディスクの同期(OS の API)を行うこともあります。このラボは標準 C++ の範囲で例外の経路だけを扱います。

---

# 第8部: シリアライズ (JSON/XML)


## Lab 62 — `json::operator[]` がないキーを null で作る
```
間違った結果:   召喚ボタン有効(期待は無効)、保存し直したセーブに "pet":null が追加
止めた位置:     petLabel の前後、ウォッチ save.size() 3 → 4
最初にずれた行: if (save["pet"].is_null())
本当の原因:     const でない json の [] は、ないキーを null で挿入する(std::map::operator[] と同じ)
```
**修正:** `if (!save.contains("pet"))`(読むだけの関数は `const json&` で受けて `contains`/`find`/`at`)
**教訓:** 「あるか確認する」と「取り出す」を区別します。const json にないキーで `[]` を使うのは未定義動作(Debug ではアサーション)なので、const にするだけでなく `contains` で確認します。

## Lab 63 — `value("crit", 1)` が int で読んで1.5を1に切り捨てる
```
間違った結果:   短剣のクリティカル30(期待45)。斧(整数2)と杖(なし)は正常
止めた位置:     loadWeapon の return、条件 w.name == "단검"(短剣) → w.crit == 1
本当の原因:     value() の戻り値の型は既定値1(int)が決める。nlohmann は小数 → 整数の変換で例外を出さずに切り捨てる
```
**修正:** `w.crit = j.value("crit", 1.0);`(または `j.value<double>("crit", 1)`)
**教訓:** ライブラリの変換関数は「型をどこで決めるか」を確認します。`get<int>()` も1.5を黙って1に切り捨てます。整数で保存されていた値を小数に変えるデータパッチでよく起きます。四捨五入のその場しのぎは1.75、2.25でまた間違えます(隠しテスト)。

## Lab 64 — `json` がキーを並べ替えて順番が消える
```
間違った結果:   読み込んだ順番 "번개 얼음창 치유 화염구"(サンダー、アイススピア、ヒール、ファイアボール)(期待 "화염구 얼음창 번개 치유")、保存後も並べ替えられる
止めた位置:     loadSkillBar の for、j を展開するとすでに並べ替えられている
本当の原因:     nlohmann::json のオブジェクトは std::map(キー順)に入る。JSONの規格もオブジェクトのキーの順番に意味を持たせない
```
**修正:** `using Json = nlohmann::ordered_json;`(読み込みと保存が一緒に変わる)
**教訓:** 順番に意味があるデータは配列で保存するのが定石です。オブジェクトのキーの順番に頼ると、ライブラリや言語を変えるたびに壊れます。ハングルや日本語のキーは UTF-8 のバイト順で並ぶので、「辞書順のように見えて実はそうではない」結果になることもあります。

## Lab 65 — UTF-8をバイトで切って `dump` が例外を投げる
```
間違った結果:   "Lv99용감한전사"(Lv99勇敢な戦士)の保存失敗: [json.exception.type_error.316] incomplete UTF-8 string; last byte: 0x95
止めた位置:     catch ブロック → clampName の return で最後のバイトを確認(0xED 0x95 = 「한」の先頭2バイト)
本当の原因:     4 + 3 + 3 = 10バイトの後の「한」(3バイト)の途中、12バイト目で切れた
```
**修正:**
```cpp
size_t cut = kMaxNameBytes;
while (cut > 0 && (static_cast<unsigned char>(name[cut]) & 0xC0) == 0x80)
    --cut;
return name.substr(0, cut);
```
**教訓:** バイト数の制限は文字の境界で切ります(後続バイトは `10xxxxxx`)。`dump` のエラーハンドラー(`error_handler_t::replace/ignore`)で例外だけを止めると、壊れた名前が画面とサーバーに残ります。CP949で読んだ文字列をそのまま入れても同じ例外(type_error 316)になります。原因は「JSONにUTF-8ではないバイトが入った」で同じです。

## Lab 66 — 既定精度6桁で float の往復が壊れる
```
間違った結果:   保存した文字列 {"x":1234.57,"y":-87.6543,"z":3.33333} → 読み込んだ位置が 1234.56995 などにずれる
止めた位置:     savePosition の return(out.str())、デバッガーの p.x 表示(1234.56775)と比較
本当の原因:     ostream の既定精度は有効数字6桁。float を正確に復元するには max_digits10(9)桁が必要
```
**修正:** `out << std::setprecision(std::numeric_limits<float>::max_digits10);`(`<limits>`)または json オブジェクトに入れて `dump()`
**教訓:** 人が読む出力と、読み直す保存は別物です。保存は往復(round-trip)が保証される形式で。`std::fixed` は小数点以下の桁数なので、0.0003141 のような小さい値を壊します。隠しテストには7~8桁では足りない値も入れてあります(MSVC と clang で同じ結果を確認)。

## Lab 67 — 属性を子要素として読んで空のノードの0
```
間違った結果:   すべてのモンスターのHPが0(攻撃力は正常)
止めた位置:     m.hp = ... の行、ウォッチ node.child("hp").empty() == true、node.attribute("hp").value() == "320"
本当の原因:     hp は属性(hp="320")なのに child("hp") で探した → 空のノード → as_int() は0
```
**修正:** `m.hp = node.attribute("hp").as_int();`
**教訓:** pugixml はないノード/属性に例外ではなく空のオブジェクトを返します。タイプミス、大文字小文字("Attack"/"attack")、属性と子要素の取り違えはすべて黙った0になります。必須の値は `if (!attr)` で確認し、データのエラーとして知らせます。

## Lab 68 — エスケープせずに連結したXML
```
間違った結果:   ギルド一覧の読み込み失敗: Error parsing element attribute → 0件(期待3)
止めた位置:     loadGuilds の失敗側、xml.c_str() + result.offset → "盾>..." の部分
本当の原因:     名前 "<剣と盾>" の < が新しいタグの始まりとして解釈された
```
**修正:** 名前を入れる前に `&` → `&amp;`(いちばん最初に)、`<` → `&lt;`、`>` → `&gt;`、`"` → `&quot;`、`'` → `&apos;`(または pugixml で文書を組み立てて `doc.save()`)
**教訓:** 文字列の連結で構造化された形式(XML、JSON、SQL、HTML)を作るとエスケープを忘れます。もっと怖いのはエラーにならない場合です: 名前 "I &lt;3 PvP" は黙って "I <3 PvP" に変わります(隠しテスト)。pugixml は対応のない `&` を大目に見るので "Tom & Jerry" は読めますが、他のパーサーではエラーです。
**参考(セキュリティ):** 外部から受け取ったXMLを読むときは、外部エンティティ(XXE)とエンティティ展開爆弾に注意します。pugixml はDTD内のエンティティを処理しないのでこの二つの攻撃には当たりませんが、他のパーサー(libxml2、MSXML、.NET の昔の既定値)は設定によってファイルを読んだりメモリを使い切ったりすることがあります。

---

# 第9部: ポインターの誤用

## Lab 69 — ローカル変数のアドレスを返す
```
ウォッチ:     board[0], board[1], board[2] がすべて同じアドレス
本当の原因:   formatEntry がローカル配列 line のアドレスを返す。関数が終わると line は消え、
             次の formatEntry 呼び出し(と他の関数呼び出し)が同じスタックの場所を再び使う。
```
**修正:** 値で返します。`std::string formatEntry(...)`、`std::vector<std::string> buildBoard(...)`。(`static char line[64]` に変えると壊れはしませんが、すべての行が最後の値で同じになります。)
**構成ごと:** Debug と Release はどちらも文字化け。MSVC の ASan は既定の設定では捕まえられません。Clang の ASan は `stack-use-after-scope` で捕まえることもあります(このリポジトリで確認)。いちばん早い手がかりはコンパイラーの警告(MSVC C4172、Clang `-Wreturn-stack-address`、GCC `-Wreturn-local-addr`)でした。
**教訓:** ポインターを返すときは「指している対象は関数より長生きするか?」をまず問います。

## Lab 70 — 解放されたモンスターから懸賞金を読む (use-after-free)
```
おかしな値:     hunter.target->bounty == -286331154 (0xEEEEEEEE = メモリチェッカーが埋めた解放済みヒープ)
最初にずれた行: field.removeDefeated()  (ターゲットのモンスターを delete)
本当の原因:     hunt が戦場を片付けた後で hunter.target から懸賞金を読む
```
**修正:** 懸賞金を先に受け取ってから片付けます。
```cpp
hunter.gold += hunter.target->bounty;
hunter.target = nullptr;
field.removeDefeated();
```
もっと堅牢にするなら、生ポインターを持ち続けずに ID で探し直すか、モンスターを `shared_ptr` で保持してターゲットは `weak_ptr` で持ちます。
**構成ごと:** Debug と Release はメモリチェッカーが `0xEE` で埋めるので FAIL(ゴールド -572662308)。チェッカーのない本物のプログラムなら、解放されたメモリに古い値が残って **PASS** することもあります(チェッカーを入れる前の MSVC Release がそうでした)。ASan は heap-use-after-free とともに、読んだ場所(`hunt`)、解放した場所(`removeDefeated`)、確保した場所(`spawn`)の呼び出し履歴を見せてくれます。
**教訓:** Release で通ったからといってバグがないわけではありません。解放されたメモリを読むバグは、見分けのつく埋め値(チェッカーの `0xEE`、MSVC デバッグヒープの `0xDD`)と ASan であぶり出します。

## Lab 71 — new[] で作って delete で消す
```
止まった場所: [CRASH] メモリ検査: new で受け取ったアドレスではない場所を解放した ← operator delete ← ~Explosion の delete particles_
観察:        最初の爆発が終わるとき ~Particle() が16回ではなく1回しか呼ばれない
             (char*)particles_ - 8 の位置に配列の個数16が保存されている(主要な ABI で共通、規格が決めたものではない)
本当の原因:   new Particle[count] で作った配列を delete(配列ではない形)で消した
```
**修正:** `delete[] particles_;` もっと良いのは `std::vector<Particle>` や `std::unique_ptr<Particle[]>` に変えて、自分で消さないことです。
**構成ごと:** Debug と Release はメモリチェッカーがその場で `[CRASH]`。チェッカーがなければ MSVC Debug は CRT ヒープのアサーション、Release はヒープ破損(Windows `0xC0000374`、glibc `free(): invalid pointer`)で落ちます。ASan は "attempting free on address which was not malloc()-ed"。デストラクターのない型(`int` など)なら先頭の個数の欄がないので、Debug と Release では何の症状もなく通ることもあります。それでも未定義動作です。
**教訓:** デストラクターを持つ型の配列では、`new[]` が先頭にこっそり個数を書いておきます。作り方と消し方はいつも対にそろえます。

## Lab 72 — ポインター演算の単位の間違い
```
ウォッチ:     (const char*)entries - (const char*)header == 16  (ヘッダーの直後なら4)
本当の原因:   header は PacketHeader*。header + sizeof(PacketHeader) は「4バイト後」ではなく
             「PacketHeader 4個分後」= 16バイト後
```
**修正:** `reinterpret_cast<const LootEntry*>(header + 1)`、またはバイト単位で `reinterpret_cast<const LootEntry*>(packet.data() + sizeof(PacketHeader))`。
**構成ごと:** Debug と Release は、パケットの直後にメモリチェッカーが置いた境界バイト `0xBDBD`(= 48573)を個数として読み、合計が145722になります。チェッカーがなければゴミ値。ASan は heap-buffer-overflow(READ of size 2)でその行で止まります。
**教訓:** ポインターに足す数の単位は「指す型1個」です。バイト単位で動かすなら `char*`/`uint8_t*` で計算します。

## Lab 73 — 初期化されていないポインターメンバー
```
止まった場所: Hero::attackPower の power += weapon_->bonus  (読み取りアクセス違反)
ローカル:    this->weapon_ == 0xa5a5a5a5a5a5a5a5
本当の原因:   武器を受け取らないコンストラクターが weapon_ を初期化しない。
             if (weapon_ != nullptr) のチェックは 0xA5A5... の前では何の役にも立たない。
```
**修正:** メンバーの宣言で初期化します。`const Weapon* weapon_ = nullptr;`
**構成ごと:** パーティーは再利用メモリ(`bughunt::recycledStorage`、`0xA5` で埋められている)の上に作られるので、Debug と Release のどちらでも毎回同じ場所で落ちます(Windows はアクセス違反、POSIX は `SIGSEGV`)。再利用メモリでなければ、残ったゴミによって通ったり、とんでもない攻撃力が出たりします。ゴミのアドレスがどこを指すかによって、stack-buffer-overflow のような見当違いの名前のレポートが出ることもあります。ASan は「初期化されていない値」そのものを捕まえる道具ではありません。
**教訓:** x64 で `0xA5A5...` のように正規アドレスでない値を読むと、例外メッセージには `0xFFFFFFFFFFFFFFFF` と出ることがあります。例外メッセージよりデバッガーのポインター値を見ます。ラボ12のように「メンバーを初期化していない」静的解析の警告が前もって教えてくれます。

## Lab 74 — shared_ptr の循環参照
```
ローカル:    guild [strong ref=4]  (ローカル変数1 + ギルドメンバー3人の guild メンバー)
本当の原因:   Guild → members(shared_ptr) → Member → guild(shared_ptr) → Guild
             互いにつかみ合っているので、最後のローカル変数が消えても参照カウントが0にならない。
```
**修正:** ギルドメンバーはギルドを所有せず、見ているだけにします。
```cpp
std::weak_ptr<Guild> guild;
...
std::string guildNameOf(const Member& member)
{
    const auto guild = member.guild.lock();
    return guild ? guild->name : "(없음)";   // (なし)
}
```
**構成ごと:** 三つの構成すべてで同じように FAIL です。クラッシュも、アサーションも、ASan のレポートもありません(Windows の ASan にはリーク検出器がありません。Linux・macOS の ASan では LeakSanitizer がプログラム終了時にリークを報告することがあります)。第2部ラボ16のように、インスタンス数やデストラクターのブレークポイントで確認します。
**教訓:** スマートポインターでも、所有関係に輪ができればリークします。「誰が誰を所有するか」を一方向に決め、逆向きのほうは `weak_ptr` にします。

## Lab 75 — [&] ラムダが終わった関数のローカル変数を参照する
```
ブレークポイント: ラムダ内の hero.attack -= bonus; (Kael の番)
ローカル:    bonus = 45   ← 激怒をかけたときの出力は (+30)
呼び出し履歴: TurnTimers::advance → ラムダ。castRage はもう終わってスタックにない
ウォッチ:     &bonus が castRage の中で見た &bonus と同じアドレス → 終わった関数のスタックの場所。
             その場所を後で Doran の castRage が上書きしたので、Doran の bonus(45)が入っている
本当の原因:   [&] がローカル変数 bonus を参照でキャプチャしたが、ラムダはタイマーに預けられて castRage より長生きする
```
**修正:** 関数より長生きするラムダはローカル変数を値で持っていきます。`hero` は呼び出し側がずっと持っているので参照のままにします。
```cpp
timers.after(3, [&hero, bonus] {
    hero.attack -= bonus;
    ...
});
```
**構成ごと:** 間違った値はコンパイラーと構成ごとに違います(このリポジトリでの Kael の例: MSVC Debug -124(ビルドし直すと変わることがある)、MSVC・Clang Release 45、Clang Debug 85、ASan はゴミ値)。上のウォッチは Release で45が出た理由です。既定の ASan は stack-use-after-return を捕まえません。Clang の ASan は `ASAN_OPTIONS=detect_stack_use_after_return=1` で実行すると、このラムダの行で `stack-use-after-return` で止まります(README.md「ラムダキャプチャと ASan」)。
**教訓:** `[&]` はラムダが **今この場で** 実行されるときだけ安全です。コールバック、タイマー、スレッド、後で実行するリストに入れるラムダはキャプチャリストを一つずつ書き、何を参照で持っていくかを確認します。`hero` を参照で持っていくのも「Hero がタイマーより長生きする」という約束に頼っています。

## あふれ: スタックとヒープの境界 (ラボ76~80)


## Lab 76 — 16 MB のローカル配列でスタックオーバーフロー
```
間違った結果:   CRASH、スタックオーバーフロー(0xC00000FD / ASan stack-overflow)
止めた位置:     countReachable の最初の行(またはスタックを事前に触る補助関数)。呼び出し履歴は lab → countReachable の数行だけ
本当の原因:     int dist[2048][2048] = 16 MB のローカル変数。関数に入った直後にスタック(Windows 1 MB)を超える。マップの大きさとは関係ない
```
**修正:** `std::vector<std::vector<int>> dist(map.height, std::vector<int>(map.width, -1));`(または1次元の `std::vector<int>` で `y * width + x`)
**教訓:** スタックオーバーフローがいつも再帰とは限りません。呼び出し履歴が短いのにスタックオーバーフローなら、大きなローカル変数を疑います。大きさが実行中に決まるデータや数十 KB を超えるデータはヒープ(`vector`)に。`static` 配列に変えると落ちはしませんが、16 MB をずっと抱え込み、複数のスレッドから使えなくなります。

## Lab 77 — フレームごとに 96 KB のバッファー × 再帰160段
```
間違った結果:   CRASH、スタックオーバーフロー。「部屋8番を生成」まで出力されて落ちる(部屋5個の試験ダンジョンは通る)
止めた位置:     buildRooms が十段ほど積まれた呼び出し履歴。隣り合う二つのフレームの &layout の差 ≈ 98,000バイト
本当の原因:     再帰は160段で終わるが、1段が 96 KB なので160段で 15 MB。バッファーは再帰呼び出しの後でも使われるので小さくできない
```
**修正:** `std::vector<char> layout(kLayoutBytes);`(フレームには vector 一つだけが残ります)。または宝箱を再帰の前に数えてバッファーを早めに使い終えるか、再帰をループにします。
**教訓:** スタックの使用量は「フレームの大きさ × 深さ」です。深さがデータ(部屋の数、木の高さ)で決まる再帰は、スタックの上限を考える必要があります。フレームの大きさは構成ごとに違います(Debug はチェック用の領域、ASan は変数の間の境界)。そのため、フレームが小さく深さが境界ぎりぎりの再帰は、Debug でだけ、あるいは ASan でだけ落ちることもあります。自分で確かめるなら、`kLayoutBytes` を 8 KB に減らして部屋の数を増やし、Debug と Release で落ちる部屋の番号を比べてみてください。

## Lab 78 — `new Frame[n]` の後で `<=` により1つ余分に書く、解放時に表に出る
```
間違った結果:   CRASH、[CRASH] メモリ検査: 確保したサイズを超えて書いた(ASan: heap-buffer-overflow)
止めた位置:     ~ReplayRecorder の delete[]。データ ブレークポイント(&frames_[ticks_])を置くと recordAll の frames_[t] = ... で t == 600
本当の原因:     for (t = 0; t <= ticks_; ++t) が要素600個の配列の frames_[600] に書き込む
```
**修正:** `for (int t = 0; t < ticks_; ++t)`
**教訓:** ヒープのあふれは書いた瞬間ではなく、その場所を他の誰かが見るとき(解放、次の割り当て、隣のブロックの使用)に表に出ます。クラッシュの位置と原因が離れていたら、**データ ブレークポイント** で「そのバイトを誰が変えるか」を尋ねます。ASan は書いた瞬間に捕まえます。ラボ72(ポインター演算の単位、読み込み)と違い、このラボはループの境界での1つ余分な書き込みです。

## Lab 79 — `memcpy` に `sizeof(ポインター)`
```
間違った結果:   1番目の名前 "Nightingnd_Archer"(期待 "Nightingale")、2番目の名前 "Moonshadnd_Archer"。先頭8文字の後ろに、直前の接続者の名前(Silverwind_Archer)の残りがくっつく
止めた位置:     writeName、ウォッチ sizeof(dst) == 8、std::strlen(src) == 11。メモリ ウィンドウで packet.name の9バイト目から前の人の名前
本当の原因:     引数 char* dst は配列ではなくポインターなので sizeof は8。先頭8バイトだけコピーされ、ヌル文字がない
```
**修正:**
```cpp
void writeName(char* dst, std::size_t cap, const char* src)
{
    const std::size_t n = std::min(std::strlen(src), cap - 1);
    std::memcpy(dst, src, n);
    dst[n] = '\0';
}
// 呼び出し: writeName(packet.name, sizeof(packet.name), name);
```
**教訓:** 配列は関数の引数になった瞬間にポインターに変わります(配列の減衰)。大きさは一緒に渡すか、`char (&dst)[N]` のテンプレートで受けます。C 文字列のコピーは「長さ + ヌル文字1」と「バッファーの大きさ - 1 で切り詰め」の両方が必要です。clang は `-Wsizeof-pointer-memaccess` ですでに警告します。パケットの外では `std::string`。

## Lab 80 — 1から数えるスキル番号で `std::array` のインデックス、隣のメンバー `gold` を上書き
```
間違った結果:   Debug: CRASH、"array subscript out of range" / Release: 2番スキルのレベル0、ゴールド801(期待800)
止めた位置:     Debug: learnSkill の hero.skillLevels[skillId] の行(アサーション) / Release: hero.gold のデータ ブレークポイントが同じ行で止まる
本当の原因:     画面の番号1~4をそのままインデックスに使った。skillLevels[4] は配列の直後の gold
```
**修正:** `hero.skillLevels[skillId - 1] += 1;`
**教訓:** `operator[]` は範囲をチェックしません。Debug 構成の標準ライブラリのチェックは保険にすぎず(Release CRT でビルドした ASan にはありません)、同じオブジェクトの中のはみ出しは ASan もたいてい捕まえられません。確信のないインデックスには `at()`。「どこで変わったのか分からない値」にはデータ ブレークポイント(ラボ18)。

---

# 第10部: 非同期と並行性

## Lab 81 — 捨てた future が非同期を逐次実行にする
```
メインスレッド:  startLoading の for 文 → std::future<void>::~future → タスクの終了を待つ
ワーカースレッド:  一度に 1 つだけが ChunkDisk::read の中にいる (300 ms 待ってからコールド読み込み)
本当の原因:    std::async の戻り値を保存しなかったため、一時 future がその行の終わりで破棄される。
              std::launch::async で作った future のデストラクターはタスクが終わるまで待つ
```
**修正:** `jobs_.push_back(std::async(std::launch::async, [this, i] { chunks_[i] = disk_.read(i); }));` こうすると `finishLoading` が集めておいた future を待ちます。
**構成別:** どのコンパイラーでも同じです(標準が定めた動作)。C++20 から `std::async` は `[[nodiscard]]` なので、警告(MSVC C4858、GCC・Clang `-Wunused-result`)がこの行を指します。
**教訓:** 「非同期で起動した」と「待たない」は別物です。future をどこに置き、いつ待つかがコードの実行順序を決めます。

## Lab 82 — 非同期タスクの例外は get() から出てくる
```
例外がスローされたときに停止:  ワーカースレッドの craft("Mithril Helm") の throw
abort 時点の呼び出し履歴:   メインスレッド craftAll → std::future<int>::get → 例外の再スロー
本当の原因:             try/catch が std::async の呼び出しだけを囲んでいた。タスク内の例外は future に格納され、get() でスローされる
```
**修正:** `get()` を try の中に移します。
```cpp
for (std::future<int>& job : jobs)
{
    try { report.revenue += job.get(); }
    catch (const std::exception& e) { std::cout << i18n::L("  제작 실패: ", "  製作失敗: ") << e.what() << '\n'; ++report.failed; }
}
```
(`std::async` 自体もスレッドを作れなければ `std::system_error` をスローするので、元の try を残しておいてもかまいません。)
**教訓:** 非同期タスクの例外は**結果を取り出す場所**で処理します。`get()` は 1 つの future につき 1 回しか呼べません。2 回目の呼び出しの前には `valid()` が false で、そのとき `get()` を呼ぶのは未定義動作です(ほとんどの実装は `std::future_error` をスローします)。

## Lab 83 — 条件なしの condition_variable の待機
```
すべて中断:  レイドメンバーのスレッドはすべて終了。lab-worker(レイドリーダー)は ReadyCheck::waitForAll の wait の中
ウォッチ:     readyCount_ == 4, memberCount_ == 4  → 待つ理由がない
本当の原因:  notify_one はリーダーが wait する前にすでに 4 回呼ばれていた。通知は保存されないので消えてしまった
```
**修正:** `allReady_.wait(lock, [&] { return readyCount_ >= memberCount_; });`
条件がすでに真なら待たず、見せかけの起床(spurious wakeup)でも条件が偽なら再び待ちます。
**構成別:** どの構成でも TIMEOUT。`if (readyCount_ < memberCount_) allReady_.wait(lock);` のように 1 回だけ検査すると画面上は通りますが、リーダーが先に待つ隠しケースで最初の通知で起きてしまい、3 人中 1 人だけが準備した状態で入場します(FAIL)。
**教訓:** `condition_variable` は「何かが変わったかもしれない」という合図にすぎません。待つ**条件**は mutex で守られた変数にあり、`wait` は常にその条件と一緒に使います。

## Lab 84 — detach したスレッドの [this]
```
ブレークポイント:     board.post(bid_)  (ワーカースレッド。呼び出し履歴に bidAndCloseWindow はない)
ウォッチ:     bid_ == -286331154 (0xEEEEEEEE = 解放済みメモリ)
本当の原因:  ラムダが this をキャプチャしたが、AuctionWindow(unique_ptr)は bidAndCloseWindow の終了時に削除された。
            ラムダは締め切りのベルが鳴った後、ウィンドウが消えた後に実行される
```
**修正:** 必要な値だけをコピーして持っていきます。`std::thread([bid = bid_, &bell, &board] { bell.waitForRing(); board.post(bid); }).detach();`
(ベルと掲示板はサーバーが動いている間ずっと生きているオブジェクトなので参照のままにします。その保証がなければ `shared_ptr` で共同所有します。)
スレッドを detach しない設計もありますが、条件が付きます。ウィンドウに `std::jthread` を所有させてデストラクターで待たせると、このラボのようにベルが鳴る前にウィンドウが閉じられる流れでは、デストラクターがベルを待って止まります(TIMEOUT)。その方法はベルが鳴った後にウィンドウが閉じられるときだけ正しいです。この流れでは通知をサーバー側のオブジェクトに登録するほうが合っています。
**構成別:** Debug と Release はメモリチェッカーの `0xEE` で FAIL。ASan はワーカースレッドで heap-use-after-free(解放した場所はメインスレッドの `bidAndCloseWindow`)として CRASH。
**教訓:** `detach` は「このスレッドの寿命には誰も責任を持たない」という意味です。detach したスレッドに渡すラムダは、キャプチャしたものがすべてそのスレッドより長く生きるか確認します(第9部のラボ 75 と同じミスがスレッドに移ってきたもの)。

## Lab 85 — atomic の確認後に行動するレース
```
条件付きブレークポイント:  --stock_; で stock_ <= 0 のときに停止
スレッド一覧:    5 つの購入スレッドがすべて stock_.load() > 0 の検査を通過し、payment_.charge で待っていた
本当の原因:      「残っているか確認 → 決済 → 1 つ減らす」がアトミックではない。atomic の一つ一つは安全だが、
                確認と減算の間に他のスレッドがすべて確認を終えてしまうことがある
```
**修正:** 決済する前に数量をアトミックに確保します。
```cpp
int left = stock_.load();
do
{
    if (left <= 0)
        return false;
} while (!stock_.compare_exchange_weak(left, left - 1));
payment_.charge(player, price_);
++sold_;
return true;
```
(`fetch_sub` で減らして結果が 0 以下なら元に戻す方法もあります。決済が失敗しうるなら、失敗したときに数量を戻します。)`buy` 全体を `std::mutex` で囲んでも正しいですが、決済を待つ間ほかの購入者が全員行列に並ぶことになります。
**構成別:** どの構成でも販売 5、数量 -2。ラボ 17 と違って**データ競合ではありません。**すべてのアクセスが `atomic` なので ThreadSanitizer も報告しません。
**教訓:** 不変条件(「販売数 + 残り数量 = 最初の数量」「数量 ≥ 0」)は、それを構成する値全体を一度に変える操作で守ります。変数ごとに `atomic` を付けても不変条件は守られません。

## Lab 86 — join せずに破棄された std::thread
```
呼び出し履歴:  std::terminate ← std::thread::~thread ← runMatchmaking の return result; (2 つ目のキュー)
本当の原因:  人数が足りずにキャンセルする経路が matcher.join() なしで関数を抜ける。
            joinable な std::thread のデストラクターは std::terminate を呼ぶ
```
**修正:** キャンセル経路でも待ちます。結果はスレッドが終わった後で直します(まだ動いているスレッドが `result.matches` を書いているため)。
```cpp
cancel = true;
matcher.join();
result.cancelled = true;
result.matches = 0;
return result;
```
`std::jthread` に替えると、すべての抜け道で自動的に停止を要求して join します。ただし jthread は関数の終わりで join するので、上のように「止めて待ってから結果を直す」必要があるなら `request_stop()` と `join()` を自分で呼びます。
**教訓:** スレッドオブジェクトもリソースです。例外や早期 `return` を含むすべての経路で後始末されるよう RAII(`std::jthread`)を使います。

## Lab 87 — コルーチンの参照引数
```
ブレークポイント:     talkTo の最初の行 (呼び出し履歴: as07_cutscene → Cutscene::step → コルーチンの再開。startVillageScene はすでに終了)
ウォッチ:     npc.affinity == -286331154 (0xEEEEEEEE)
本当の原因:  talkTo は initial_suspend ですぐに中断し、コルーチンフレームには npc の参照だけが保存される。
            startVillageScene が終わって Npc(unique_ptr)が削除された後に、エンジンがコルーチンを初めて実行する
```
**修正:** コルーチンが使う値はフレームの中にコピーして持っていきます。`Cutscene talkTo(Npc npc, DialogueLog& log)` (`log` は呼び出し側がカットシーンより長く保持するので参照のままにします。)
**構成別:** Debug と Release はメモリチェッカーの `0xEE` で FAIL。ASan は heap-use-after-free として CRASH。MSVC 14.51 と Clang 22 で確認しました。
**教訓:** コルーチンの参照引数は「最初に中断する場所まで」しか安全ではありません。中断して再開するコルーチンは、ラムダを後で実行するのと同じ寿命の問題を抱えます。

---

# 第11部: フリーズ - 無限ループとデッドロック

## Lab 88 — float の累積と != の終了条件
```
すべて中断:     ラボのワーカースレッド → ticksUntilReady。すべて中断するたびに ticks が増えている (無限ループ)
条件付きブレークポイント: ticks == 15 のとき t == 1.50000024, cooldownSeconds == 1.5
本当の原因:     0.1 は 2 進数で正確に表せないため、0.1f を 15 回足しても正確に 1.5f にならない。
               t != cooldownSeconds は永遠に真。(ダッシュの 0.5 秒はたまたまぴったり合って終わった)
```
**修正:** 繰り返し回数を整数で計算します。
```cpp
return static_cast<int>(std::lround(cooldownSeconds / tickSeconds));   // #include <cmath>
```
`t < cooldownSeconds` に変えるだけでも止まりはしますが、誤差のせいで 1 ティック多く数えます。たとえば 1.2 秒 / 0.05 秒は 24 ではなく 25 になります(採点スクリプトの隠しケースがこれを捕まえます)。
**教訓:** 浮動小数点数は `==`、`!=` で比較しません。「何回繰り返すか」は整数で数えます。

## Lab 89 — unsigned の s >= 0
```
すべて中断:   s が 12 よりはるかに大きい数 (数十億)。条件付きブレークポイント s == 0 で 1 行実行すると 4294967295 (0xFFFFFFFF)
本当の原因:   unsigned には負の数がないので s >= 0 は常に真。0 で --s すると 4294967295 に戻る。
```
**修正:** `for (unsigned int s = waitSeconds + 1; s-- > 0;)` または符号付き整数で数えます: `for (int s = static_cast<int>(waitSeconds); s >= 0; --s)`。
**教訓:** このミスを知らせる「式が常に true」の警告は、既定の警告レベルでは表示されないことが多いです。MSVC C4296 は `/W4` でもオフなので `/w14296` で有効にし、GCC は `-Wtype-limits`(`-Wextra` に含まれる)、Clang は `-Wtautological-unsigned-zero-compare` で有効にします。逆順に数えるループで `unsigned`/`size_t` を見たら疑います。

## Lab 90 — continue が ++i を飛ばす
```
すべて中断:   i == 6 (最初の空白), c == ' ', words == { "/party" }。1 行ずつ実行し続けても i はそのまま
本当の原因:   空白の分岐が i を増やさずに continue → 同じ空白を永遠に読み直す
```
**修正:** `continue;` の前に `++i;`。または `for (char c : line)` のようにインデックスを直接扱わないループに変えます。
**教訓:** `while` ループのすべての経路がループ変数を前に進めるか確認します。次のステートメントの設定で `++i;` を 1 回実行させてみると、仮説をすぐに試せます。

## Lab 91 — 挿入の順序が逆で自分自身を指す
```
すべて中断:   countStops の中、stops が数千万、p->name == "Forest"
ウォッチ:      p == p->next  (Forest の next が自分自身)
本当の原因:   pos->next = wp; を先に行うと、次の行 wp->next = pos->next が今入れたばかりの wp 自身を読む
```
**修正:** 順序を入れ替えます。
```cpp
wp->next = pos->next;
pos->next = wp;
```
防御するなら、巡回するときに循環を検査します(ウサギとカメ: 1 つずつ進むポインターと 2 つずつ進むポインターが出会えば循環)。
**教訓:** 連結リストで無限ループが起きたら、ウォッチに `p`、`p->next`、`p->next->next` を入れて**アドレス**を比較します。

## Lab 92 — ロック順序が食い違うデッドロック
```
並列スタック:   trade スレッド 2 つがどちらも lock_guard → mutex lock で待機 (CPU 0%)
スレッド A:    seller = Kael。Kael.lock を持ち、Mira.lock を待つ
スレッド B:    seller = Mira。Mira.lock を持ち、Kael.lock を待つ
本当の原因:   「売り手が先、買い手が後」というロック順序は取引の方向によって逆転する。
             逆方向の取引が同時に来ると、互いに相手が持っているロックを待つ (循環待機)。
```
**修正:** 2 つの mutex をまとめてロックします。`std::scoped_lock` はデッドロックを避ける順序でロックします。
```cpp
std::scoped_lock locks(seller.lock, buyer.lock);
if (!verifyItem(seller, item))
    return;
```
または常に同じ基準(プレイヤー ID が小さい方、アドレスが小さい方)で先にロックします。隠しケースの 3 人の循環取引もこの方法で解けます。
偽造チェックをロックの前に移すだけでも 50ms の隙間が縮んでほとんど通りますが、ロック順序の問題はそのままなので、ごくまれにしか止まらない、さらに見つけにくいバグになります。しかもロックなしで検査すると、検査と取引の間にアイテムが変わる可能性があります。
**教訓:** デッドロックは「誰が何を持ち、何を待っているか」をスレッドごとに書き出してみると輪が見えます。複数のロックは一度に、または常に同じ順序で取ります。

---

# 第12部: パフォーマンス

第12部はすべて「結果は正しいのに仕事をしすぎる」です。記録表の**カウント値**が修正前後でどう変わるかも併せて書きました。

## Lab 93 — vector の先頭から 1 つずつ消す (O(n²))
```
カウント値:     要素の移動 31,996,000 回 (メッセージ 8000 件) → 16000 件なら約 4 倍
ホット パス:     ChatQueue::drain → vector::erase → Message のムーブ代入 / memmove
本当の原因:   queue_.erase(queue_.begin()) は後ろの要素をすべて 1 つずつ前に詰める。n 個をこうして取り出すと n²/2 回の移動
```
**修正:** 消さずに最後まで読んでから一度に空にします。
```cpp
for (const Message& m : queue_)
    chars += static_cast<long long>(m.text.size());
queue_.clear();
```
(先頭から取り出すことが多いなら `std::deque` の `pop_front` でもかまいません。移動 0 回。)
**修正後:** 要素の移動 0 回。
**教訓:** 「先頭から 1 つずつ消す」は vector で最もよくある O(n²) です。サイズを 2 倍にして時間や回数が 4 倍になったら疑います。

## Lab 94 — 大きなオブジェクトを値で渡し、値でループする
```
カウント値:     Item のコピー 144,000 回 = 120 個 × 600 フレーム × 2
コピーコンストラクターのブレークポイントでの呼び出し履歴:  (1) drawHud → totalWeight の呼び出し(引数のコピー)  (2) totalWeight の for (Item item : items)
本当の原因:   totalWeight(std::vector<Item> items) がベクター全体をコピーし、範囲 for が要素をさらに 1 つずつコピーする。
             Item ごとに 200 文字の説明文字列があり、コピーのたびに割り当てが起きる
```
**修正:**
```cpp
int totalWeight(const std::vector<Item>& items)
...
for (const Item& item : items)
```
**修正後:** Item のコピー 0 回。どちらか一方だけ直すと 72,000 回が残ります。
**教訓:** 読むだけの大きな引数は `const&`、範囲 for も `const auto&` が基本です。

## Lab 95 — ループ内の s = s + ...
```
カウント値:     割り当て 約 250MB (結果は 58KB)。イベントを 2 倍にすると約 4 倍
ホット パス:     exportLog → operator+ → 文字列のコピー(memcpy)、割り当て/解放
本当の原因:   log = log + "[" + ... は行ごとにそれまでの log 全体をコピーした一時文字列を作り、連結するたびにさらに大きくなる。
             4000 行目を連結するときには 6 万文字が何度もコピーされる
```
**修正:** 既存の文字列の後ろに追加します(`+=` は容量が足りないときだけ拡張します)。
```cpp
log += "[";
log += toText(e.tick);
log += "] hit ";
log += toText(e.amount);
log += "\n";
```
(`log += "[" + toText(e.tick) + ... ;` のように 1 行分の一時文字列だけを作るのでも通ります。サイズがわかっているなら `log.reserve(...)` で割り当てを 1 回に減らせます。)
**修正後:** Release で割り当て 20 回、約 200KB。Debug はイテレーターデバッグ用の補助割り当てのため回数が多くなりますが、結果の長さに比例します。
**教訓:** 文字列を累積するとき、`s = s + x` は O(n²)、`s += x` は償却 O(n) です。

## Lab 96 — std::endl は毎回 flush する
```
カウント値:     flush 5002 回 (オブジェクト 5000 個 + ヘッダー行 + 最後の flush)
FakeLogFile::sync のブレークポイントでの呼び出し履歴:  writeSave の << std::endl の 2 行
本当の原因:   std::endl = '\n' + flush。行ごとにディスク書き込みを行う
```
**修正:**
```cpp
out << "SAVE v3 " << objects.size() << '\n';
...
out << o.id << ' ' << o.x << ' ' << o.y << '\n';
```
最後の `out.flush()` は残します。
**修正後:** flush 1 回。(最後の flush まで消すと 0 回になって FAIL。書き終えた後に一度は書き出す必要があります。)
**教訓:** 改行は `'\n'`、flush は必要なときに 1 回。コンソール出力も同じです。

## Lab 97 — 行優先の配列を列優先でループする
```
カウント値:     飛び越したアクセス 3,999,999 回 (ほぼすべてのアクセス)
条件付きブレークポイント (jumps_ < 5):  row が 0, 1, 2... と変わり col はそのまま → index が cols(2000)ずつ、8000 バイトずつ飛ぶ
本当の原因:   マスは 1 行ずつ連続して並んでいるのに、countFog は列を外側、行を内側のループで回る。
             毎回のアクセスが別のキャッシュラインなのでメモリから新たに読み込む。reveal は行優先なので速い
```
**修正:** ループの順序を入れ替えます。
```cpp
for (int r = 0; r < map.rows(); ++r)
{
    for (int c = 0; c < map.cols(); ++c)
```
**修正後:** 飛び越したアクセス 0 回。
**教訓:** 2 次元配列はメモリに並んだ順にループします。プロファイラーは「countFog が遅い」までしか教えてくれず、なぜ遅いかは同じ仕事をする reveal と比べて初めて見えます。

## Lab 98 — shared_ptr を値で渡し、値でループする
```
カウント値:     1 体のモンスターを同時に保持した shared_ptr 最大 3 個 (リスト + 範囲 for の m + threatOf の引数)
条件付きブレークポイント (use_count() > 1) の呼び出し履歴:  mostDangerous の for (MonsterPtr m : monsters)、threatOf(MonsterPtr monster)
本当の原因:   shared_ptr をコピーするたびに参照カウントをアトミックに増減する。3000 体 × 300 ティック × 2 回
```
**修正:**
```cpp
int threatOf(const MonsterPtr& monster)
...
for (const MonsterPtr& m : monsters)
```
(`threatOf(const Monster& monster)` のように所有権と関係ない関数はオブジェクトの参照を受け取ってもかまいません。)
**修正後:** 最大 1 個。どちらか一方だけ直すと 2 個が残ります。`best = m;` は最も危険なモンスターが変わったときだけコピーするので、そのままでかまいません。
**教訓:** 所有権を渡すとき、または共有するときだけ `shared_ptr` を値で渡します。読むだけなら `const&` かオブジェクトの参照で十分です。
