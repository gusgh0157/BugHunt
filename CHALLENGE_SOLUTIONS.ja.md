[한국어](CHALLENGE_SOLUTIONS.md) | プログラムの実行出力は `--lang ja` で日本語になります(ラボのコードが独自に出す進行メッセージも含みます。ゲーム内のデータ(名前、ファイルの内容など)と、ラボが保存して比較する文言も選んだ言語になります)。本書の出力例とデバッガーに入力する値は `--lang ja` で実行したときのものです。解答コードの文字列リテラルは、ラボのコードと同じく `i18n::L("韓国語", "日本語")` で両方の言語を書くか、原文の韓国語に(　)やコメントで訳を添えています。

# チャレンジ解答(ネタバレ注意!)

チャレンジを自分で解く前には読まないでください。問題をどの部から取ったかは一番下の表にあります。

---

# 深い追跡

3 問とも、症状が出る場所と原因がある場所の間に 4 段階以上あり、症状のすぐ近くに「直すと画面の [PASS] が出る」偽の容疑者がいます。そこを直すと、採点スクリプトの隠しケースが `症状は消えたが原因が残っている` として FAIL を出します。

## C-1 — 画面パネルがオークションハウスの参加者ストレージをソートする

```
症状:        落札者が Kael。Mira の金貨は 800 のまま(入札した 450 が引かれていない)、Kael は 150(競り負けたのに返金されていない)
1 段階:       close() が bidders_[*leader_] に品物を渡す → leader_ が指す人が Kael
2 段階:       placeBid(mira, 450) に条件付きブレークポイント(amount == 450) → bidders_[who].name が "Kael"
             → 参加番号 mira(1) が別の人を指している
3 段階:       3 回目の join の次の行にブレークポイントを置いて最初から実行。止まったらウォッチに house.bidders_[1].name、データ ブレークポイント(gdb・lldb の watchpoint)を &house.bidders_[1].gold に設定して続行
             → すぐ次の drawFrame の中で停止。呼び出し履歴: std::stable_sort ← AuctionHouse::forEachByTier ← BidderPanel::refresh ← drawFrame
4 段階:       forEachByTier が「表示するために」bidders_ そのものをランク順にソートしている。
             参加番号(BidderId = ベクター内の位置)と leader_ がどちらも別の人を指すようになる
トレースポイントの例:   placeBid の最初の行に "{who} {bidders_[who].name} {amount}" → 呼び出し側が渡した番号と実際の名前のずれが一目でわかる
```
**偽の容疑者:** `close()` の手数料計算(`TODO: 丸め方が正しいか企画に確認`)と、`placeBid` の「自分の入札を引き上げる」分岐。どちらも正しいです。受取額 428 もすでに正しく出ています。
**症状だけを隠す修正:** シナリオの `drawFrame` から `bidders.refresh(house)` を外すと画面の検査はすべて通りますが、隠しケースがパネルを描画した後に参加番号を確認するので FAIL。
**修正:** ストレージはそのままにして、表示する順序だけをソートします。
```cpp
void AuctionHouse::forEachByTier(const std::function<void(const Bidder&)>& visit)
{
    std::vector<const Bidder*> view;
    for (const Bidder& b : bidders_)
        view.push_back(&b);
    std::stable_sort(view.begin(), view.end(), [](const Bidder* a, const Bidder* b) { return a->tier > b->tier; });
    for (const Bidder* b : view)
        visit(*b);
}
```
**教訓:** 位置(インデックス)を ID として使うコンテナでは、順序を変えるあらゆる操作が ID を壊します。「表示する」関数が `const` でないなら、なぜそうでないのかを疑います。値がいつ変わるかわからないときは、データ ブレークポイントが犯人をすぐに教えてくれます。

## C-2 — ハンドラーが購読を解除すると次の購読者がイベントを飛ばす

```
症状:        「影」の称号がない、最高ノーダメージ連続撃破 5 (期待値 6)
1 段階:       TitleSystem::award → tracker_.bestStreak() が 5
2 段階:       AchievementTracker ハンドラーの ++current_ にデータ ブレークポイント(またはトレースポイント "#{e.seq} {current_}")
             → #2, #3, #5, #6, #7 は通過するのに #4(3 匹目のオオカミ)だけが抜けている
3 段階:       EventBus::publish に条件付きブレークポイント e.seq == 4、1 行ずつ追いながらウォッチに i, subscribers_.size(), subscribers_[i].name
             → i == 1 の「依頼: オオカミの群れ討伐」ハンドラーを呼んだ後に size が 5 → 4、次の i == 2 は「戦利品」
4 段階:       依頼ハンドラー内の bus_.unsubscribe(sub_) の呼び出し履歴: HuntQuest のラムダ ← EventBus::publish
             → 巡回中に vector から削除すると後ろの要素が 1 つずつ前に詰まり、i はそのまま増えるので「実績トラッカー」を飛ばす
```
**偽の容疑者:** `AchievementTracker` の「防いだ攻撃(ダメージ 0)も被弾とみなす」処理。#8 で盾で防いだら連続撃破が途切れるのが怪しく見えます。しかし企画書どおりです。
**症状だけを隠す修正:**
- ダメージ 0 なら連続撃破を途切れさせないように変えると 5 + 2 = 7 になって画面は通ります。隠しケースが実績の撃破数と企画書 4.2 を確認するので FAIL。
- 依頼が購読を解除しないように(`done_` だけを立てるように)変えると画面は通ります。隠しケースが残りの購読者数を確認するので FAIL。バスは「ハンドラー内で unsubscribe してもよい」と約束しているので、直すべき場所はバスです。
**修正:** 今回のイベントを受け取る購読者リストを先にコピーしておき、そのコピーを巡回します。
```cpp
void publish(const Event& e)
{
    const std::vector<Subscriber> snapshot = subscribers_;
    for (const Subscriber& s : snapshot)
        s.handler(e);
}
```
(削除を保留しておき、publish が終わった後にまとめて削除する方法もあります。こうすると実行中の `std::function` を上書きしてしまう問題も一緒に消えます。)
**教訓:** 「巡回中のコンテナ変更」は、イテレーターの代わりにインデックスを使っても消えません。クラッシュせずに 1 回飛ばすだけなので、値を数える側(実績)から始めて、イベントを 1 つずつトレースポイントで記録してみるのが最も速い方法です。

## C-3 — 自作のコピーコンストラクターが新しく追加されたメンバーを漏らす

```
症状:        復活後の一撃 48 (期待値 52)
1 段階:       DamageMeter::hit → StatSystem::attackOf。ウォッチに hero.prestige(), hero.buffs()
             → 復活前は prestige 2、復活後は 0
2 段階:       Aria を作った次の行で止めてデータ ブレークポイントを &aria.prestige_ に設定して続行 → prestigeUp で 2 回止まった後、respawn 内の swap で停止
             呼び出し履歴: swap ← Hero::operator= ← RespawnSystem::respawn
3 段階:       swap の直前に other.prestige_ がすでに 0。other は checkpoints_.snapshotOf(...) のコピー
             → ウォッチに checkpoints.snapshots_ → 保存された Aria も prestige_ 0
4 段階:       CheckpointSystem::save の insert_or_assign にブレークポイント、関数の中に入ると(ステップ イン (Step Into)) Hero(const Hero&)
             → 初期化リストに prestige_ がない。既定値 0 のまま残る (ペットをディープコピーするために自作したコピーコンストラクター、prestige_ はシーズン 2 で追加)
```
**偽の容疑者:** `RespawnSystem::respawn` の `hero.clearBuffs()`。減ったダメージ(48 → 52、約 10%)が消えた「雄叫び +10%」とぴったり合うように見えます。しかし企画書 7.1 どおり死んだらバフは消えるべきで、プレステージ 2 段階も +10% なので偶然同じになっただけです。
**症状だけを隠す修正:** `clearBuffs()` を消すと 0 + 10% = 52 になって画面は通ります。隠しケースがプレステージ 3、バフなしのヒーローで確認するので FAIL。
**修正:**
```cpp
Hero::Hero(const Hero& other)
    : ...
      buffs_(other.buffs_),
      prestige_(other.prestige_)
{
}
```
**教訓:** コピーコンストラクターを自作すると、メンバーを追加するたびにそこも直さなければなりません(`swap` は直したのにコピーコンストラクターは漏らした、というように)。ディープコピーが必要なメンバー 1 つのためなら、そのメンバーだけをコピー可能なラッパーで包み、クラスは既定のコピー(= Rule of Zero)を使う方が安全です。既定値のあるメンバーは漏らしても警告が出ません。

---

# ブラインド

## B-1 — 空の vector で `size() - 1`
`shelf.size() - 1` は `size_t` なので、空の棚では 0 - 1 = 18446744073709551615。ループが回って `shelf[0]` を読みます。空の vector はバッファーがない(data() が nullptr)ので、不正なアドレスへのアクセスで落ちます(MSVC の Debug STL はその前に範囲アサーション)。
**修正:** `for (size_t i = 0; i + 1 < shelf.size(); ++i)`

## B-2 — 一時文字列を指す参照を返す
`authorOr(n, i18n::L("길드 운영 위원회 (자동 공지)", "ギルド運営委員会 (自動お知らせ)"))` の 2 番目の引数は、その行が終わると消える一時 `std::string` です。返された `const std::string&` は解放済みメモリを指します。(SSO の 15 バイトより長いのでヒープにあります。短い名前だったら偶然正しく出ていたかもしれません。)
**修正:** `std::string authorOr(...)` で値を返します。

## B-3 — `uint8_t` のループ変数
`x` が 255 から 0 に戻るので、幅が 257 以上だと `x + 1 < count` が永遠に真です。
**修正:** `for (int x = 0; x + 1 < count; ++x)`

## B-4 — メンバーの代わりに引数へ代入
コンストラクター内の `maxHp = maxHp;` は引数に自分自身を代入しています(メンバーは `maxHp_`)。メンバー `maxHp_` は初期化されず、vector 内のモンスターはメモリチェッカーが埋めた `0xA5A5A5A5`(-1515870811)になります。MSVC は自己代入に警告を出しません(Clang は `-Wself-assign` で手がかりを出します。GCC は警告を出しません)。デバッガーで `this->maxHp_` を見ればすぐにわかります。
**修正:** `maxHp_ = maxHp;` (できれば初期化リストで `maxHp_(maxHp)`)

## B-5 — 範囲 for の中で同じ vector に `push_back`
最初の `push_back` で再割り当てが起きると、範囲 for のイテレーターが古いバッファーを指します。それ以降は解放済みメモリを読みます。
**修正:** 元のサイズの分だけインデックスでループし、要素はコピーしておきます。
```cpp
const size_t original = wave.size();
for (size_t w = 0; w < original; ++w)
{
    const Unit u = wave[w];
    for (int i = 0; i < u.summons; ++i)
        wave.push_back({ u.name + i18n::L("의 부하", "の手下"), 0 });
}
```

## B-6 — 重複除去を線形探索で (O(n²))
ログイン記録ごとに、それまでに集めたリスト全体と比較します。比較は約 6 千万回。
**修正:** 見たことがあるかを `std::set`(またはハッシュを定義した `unordered_set`)で確認します。最初に出てきた順序は結果の vector が保ちます。
```cpp
std::set<std::pair<int, int>> seen;
for (const PlayerId& p : logins)
{
    if (seen.insert({ p.server, p.number }).second)
        unique.push_back(p);
}
```

## B-7 — `std::accumulate` の初期値が `int`
初期値 `0` の型が累積の型になります。足すたびに `int` に切り捨てられ、2.5 + 3.75 + 1.25 + 2.5 が 8 になります。MSVC は警告 C4244 で `<numeric>` の中を指します(このリポジトリのビルドで出ます)。GCC・Clang は警告がシステムヘッダーの中で出るので、既定の設定では見えないことがあります。
**修正:** `std::accumulate(prices.begin(), prices.end(), 0.0)`

## B-8 — `[this]` ラムダを登録した後にオブジェクトが移動する
タレットを `emplace_back` するたびに vector が再割り当てされると、以前のタレットは新しいアドレスに移動します。警報に登録したラムダは古いアドレスの `this` を持ったままです。
**修正:** あらかじめ `turrets.reserve(names.size())` で移動しないようにするか、すべて設置してから接続するか、末尾にだけ追加する場合にアドレスが変わらない `std::deque` に置きます(`push_back` だけなら要素は移動しません)。`std::list` や `unique_ptr` の vector もアドレスは安定していますが、その場合は隠しテストの `turrets.at(0)` のようなアクセス方法も併せて変える必要があります。

---

# 出典

| 問題 | 取った部 | 種類 |
|---|---|---|
| C-1 | 第4部 コンテナ + 第3部 フロー追跡 | コンテナ(位置を ID として使う)、隠れた副作用 |
| C-2 | 第2部 発展(ラボ 13) + 第3部 フロー追跡(ラボ 24) | コンテナ(巡回中の変更)、フローと順序 |
| C-3 | 第2部 発展(ラボ 12, 15) + 第3部 フロー追跡 | オブジェクトの生成と破棄 |
| B-1 | 第2部(ラボ 14) + 第4部 | 数値(unsigned)、コンテナ |
| B-2 | 第2部(ラボ 11) + 第9部 | ポインターと寿命 |
| B-3 | 第11部 | 停止(無限ループ)、数値 |
| B-4 | 第2部(ラボ 12) | オブジェクトの生成と破棄 |
| B-5 | 第2部(ラボ 10, 13) | コンテナ、ポインターと寿命 |
| B-6 | 第12部(ラボ 93) | パフォーマンス |
| B-7 | 第1部(ラボ 3) | 数値 |
| B-8 | 第9部(ラボ 75) + 第2部(ラボ 10) | ラムダキャプチャ、ポインターと寿命 |
