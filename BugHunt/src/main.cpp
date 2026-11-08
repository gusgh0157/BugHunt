#include "labs.h"
#include "grader.h"
#include "platform/platform.h"

#include <cctype>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace
{
struct LabEntry
{
    i18n::Text title;
    i18n::Text symptom;
    void (*run)();
    void (*hidden)() = nullptr;   // 채점용 숨은 테스트 케이스 (없으면 nullptr)
};

constexpr LabEntry kLabs[] = {
    // 1부: 호출 스택 입문
    { { "친구 프로필", "友達のプロフィール" }, { "세 번째 프로필에서 죽는다", "3番目のプロフィールで落ちます" }, cs01_profile },
    { { "퀵슬롯", "クイックスロット" }, { "엉뚱한 아이템이 나오다가 멈춘다", "おかしなアイテムが出たあとで止まります" }, cs02_quickslot },
    { { "전투 기록 DPS", "戦闘記録のDPS" }, { "DPS 계산 중에 죽는다", "DPSの計算中に落ちます" }, cs03_battle_log },
    { { "파티 게시판", "パーティー掲示板" }, { "빈 파티는 처리했는데도 멈춘다", "空のパーティーは処理したのに止まります" }, cs04_party_board },
    { { "불꽃놀이", "花火" }, { "카운트다운이 끝나지 않고 죽는다", "カウントダウンが終わらずに落ちます" }, cs05_fireworks },
    { { "상점 계산", "ショップの会計" }, { "합계를 내다가 abort()가 뜬다", "合計を出している途中で abort() が出ます" }, cs06_shop },
    { { "HP 이벤트", "HPイベント" }, { "HP 바 갱신에서 죽는다", "HPバーの更新で落ちます" }, cs07_events },
    { { "몬스터 웨이브", "モンスターウェーブ" }, { "3번째 웨이브에서 멈춘다", "3番目のウェーブで止まります" }, cs08_waves },
    // 2부: 심화
    { { "점수 평균", "スコアの平均" }, { "실행하자마자 멈춘다", "起動するとすぐに止まります" }, lab01_average },
    { { "인벤토리 보상", "インベントリの報酬" }, { "보상으로 준 아이템 개수가 이상하다", "報酬として渡したアイテムの数がおかしいです" }, lab02_inventory },
    { { "플레이어 칭호", "プレイヤーの称号" }, { "칭호 문자열이 깨진다", "称号の文字列が化けます" }, lab03_titles },
    { { "콤보 카운터", "コンボカウンター" }, { "최고 콤보가 틀린다 (Debug/Release 차이)", "最高コンボが間違っています(Debug/Release の違い)" }, lab04_combo },
    { { "죽은 적 제거", "倒した敵の除去" }, { "적 정리 중 멈춘다", "敵の整理中に止まります" }, lab05_enemies },
    { { "배낭 용량", "バックパックの容量" }, { "꽉 찬 배낭에 아이템이 들어간다", "満杯のバックパックにアイテムが入ります" }, lab06_backpack },
    { { "패킷 버퍼", "パケットバッファー" }, { "두 번째 패킷을 넣는 순간 죽는다", "2番目のパケットを入れた瞬間に落ちます" }, lab07_packets },
    { { "상태 이상 효과", "状態異常効果" }, { "메모리가 조금씩 샌다", "メモリが少しずつ漏れます" }, lab08_effects },
    { { "길드 은행", "ギルド銀行" }, { "넣은 골드보다 금고가 훨씬 적다", "入れたゴールドより金庫がずっと少なくなります" }, lab09_guild_bank },
    { { "파티 칭호 수여", "パーティー称号の授与" }, { "아무도 때리지 않았는데 HP가 바뀐다", "誰も攻撃していないのにHPが変わります" }, lab10_party },
    { { "퀘스트 선행 조건", "クエストの前提条件" }, { "프로그램이 즉시 죽는다", "プログラムがすぐに落ちます" }, lab11_quests },
    { { "상태 플래그", "状態フラグ" }, { "독에 걸렸는데 아니라고 한다", "毒にかかっているのに、かかっていないと判定されます" }, lab12_status_flags },
    // 3부: 흐름 추적 (크래시 없음)
    { { "출석 보상", "出席報酬" }, { "7일 연속 출석했는데 보너스가 없다", "7日連続で出席したのにボーナスがありません" }, ft01_attendance,   ft01_attendance_hidden },
    { { "치명타 피해", "クリティカルダメージ" }, { "치명타가 일반 공격과 똑같이 들어간다", "クリティカルが通常攻撃と同じダメージになります" }, ft02_critical,     ft02_critical_hidden },
    { { "보호막 탱커", "シールドタンク" }, { "피해 합계가 맞지 않는다", "ダメージの合計が合いません" }, ft03_shield,       ft03_shield_hidden },
    { { "늑대 사냥 퀘스트", "オオカミ狩りクエスト" }, { "5마리를 잡았는데 퀘스트가 안 끝난다", "5匹倒したのにクエストが終わりません" }, ft04_quest,        ft04_quest_hidden },
    { { "상점 할인", "ショップの割引" }, { "VIP가 VIP 할인을 못 받는다", "VIPがVIP割引を受けられません" }, ft05_discount,     ft05_discount_hidden },
    { { "장비 수리", "装備の修理" }, { "골드는 빠졌는데 내구도가 그대로다", "ゴールドは減ったのに耐久度がそのままです" }, ft06_repair,       ft06_repair_hidden },
    { { "업적 해금", "実績の解除" }, { "조건을 채운 업적 하나가 안 열린다", "条件を満たした実績が1つだけ解除されません" }, ft07_achievements, ft07_achievements_hidden },
    { { "난이도 설정", "難易度設定" }, { "어려움으로 저장했는데 적이 약하다", "「むずかしい」で保存したのに敵が弱いです" }, ft08_difficulty,   ft08_difficulty_hidden },
    // 3부 끝: 연산자와 평가 순서
    { { "길드 회원 번호", "ギルドメンバー番号" }, { "새 회원이 기존 회원과 같은 번호를 받는다", "新メンバーが既存メンバーと同じ番号をもらいます" }, ft09_member_ids, ft09_member_ids_hidden },
    { { "최고 콤보", "最高コンボ" }, { "기록을 갱신할 때마다 콤보가 2씩 오른다", "記録を更新するたびにコンボが2ずつ増えます" }, ft10_best_combo, ft10_best_combo_hidden },
    { { "어둠의 동굴", "闇の洞窟" }, { "열쇠도 횃불도 있는데 입장을 못 한다", "鍵もたいまつもあるのに入れません" }, ft11_cave_gate,    ft11_cave_gate_hidden },
    { { "자물쇠 따개", "ロックピック" }, { "열쇠가 있는데도 따개가 줄어든다", "鍵を持っているのにロックピックが減ります" }, ft12_lockpicks,    ft12_lockpicks_hidden },
    // 4부: 컨테이너
    { { "가방 조회", "バッグの照会" }, { "상점을 열었을 뿐인데 가방이 꽉 찬다", "ショップを開いただけなのにバッグが満杯になります" }, ct01_bag_lookup,   ct01_bag_lookup_hidden },
    { { "파티 찾기", "パーティー募集" }, { "고른 사람과 다른 사람이 초대된다", "選んだ人とは別の人が招待されます" }, ct02_party_finder, ct02_party_finder_hidden },
    { { "아레나 순위표", "アリーナランキング" }, { "순위표를 정렬하다가 멈춘다", "ランキングをソートしている途中で止まります" }, ct03_leaderboard,  ct03_leaderboard_hidden },
    { { "장비 정리", "装備の整理" }, { "부서진 장비를 버렸는데 가방이 그대로다", "壊れた装備を捨てたのにバッグがそのままです" }, ct04_repair_shop, ct04_repair_shop_hidden },
    { { "던전 문 퍼즐", "ダンジョンの扉パズル" }, { "퍼즐 점수가 거꾸로 매겨진다", "パズルのスコアが逆に付きます" }, ct05_dungeon_doors, ct05_dungeon_doors_hidden },
    { { "이름표", "ネームプレート" }, { "칭호를 받으면 이름표가 깨진다", "称号をもらうとネームプレートが化けます" }, ct06_nameplates,   ct06_nameplates_hidden },
    { { "시즌 랭킹", "シーズンランキング" }, { "우승자가 1위에 오르지 않는다", "優勝者が1位になりません" }, ct07_season_ranking, ct07_season_ranking_hidden },
    // 5부: C++ 언어 함정
    { { "아이템 거래", "アイテムトレード" }, { "거래한 아이템이 내 거래 창에 그대로 남는다", "トレードしたアイテムが自分のトレード画面にそのまま残ります" }, lg01_trade_items, lg01_trade_items_hidden },
    { { "경험치 상한", "経験値の上限" }, { "이벤트 경험치를 받았더니 경험치가 음수가 된다", "イベント経験値をもらったら経験値が負の数になります" }, lg02_exp_cap, lg02_exp_cap_hidden },
    { { "환전소", "両替所" }, { "은화가 1개 모자라고, 큰 묶음 판매액은 음수가 된다", "銀貨が1枚足りず、大口の売上額が負の数になります" }, lg03_exchange, lg03_exchange_hidden },
    { { "레벨 보정", "レベル補正" }, { "파티 평균보다 레벨이 높은 사람이 최대 보너스를 받는다", "パーティー平均よりレベルが高い人が最大ボーナスをもらいます" }, lg04_level_bonus, lg04_level_bonus_hidden },
    { { "기본 스탯표", "基本ステータス表" }, { "게임을 켤 때 어떤 스탯이 0이다", "ゲーム起動時にあるステータスが0になっています" }, lg05_stat_table,   lg05_stat_table_hidden },
    { { "저장 화면", "セーブ画面" }, { "자동 저장을 했는데 저장 횟수가 0이다", "オートセーブしたのにセーブ回数が0です" }, lg06_save_screen,  lg06_save_screen_hidden },
    { { "던전 불러오기", "ダンジョン読み込み" }, { "불러오기에 실패할 때마다 텍스처가 남는다", "読み込みに失敗するたびにテクスチャーが残ります" }, lg07_dungeon_load, lg07_dungeon_load_hidden },
    { { "세이브 파일 닫기", "セーブファイルを閉じる" }, { "저장 실패 메시지 대신 게임이 꺼진다", "セーブ失敗のメッセージの代わりにゲームが終了します" }, lg08_save_close,   lg08_save_close_hidden },
    { { "연계 퀘스트", "連続クエスト" }, { "연계 퀘스트가 한 차례에 한 단계씩만 열린다", "連続クエストが1ターンに1段階ずつしか開きません" }, lg09_quest_chain, lg09_quest_chain_hidden },
    // 6부: 인터페이스와 상속
    { { "백스탭", "バックスタブ" }, { "등 뒤에서 찔러도 일반 피해만 들어간다", "背後から刺しても通常ダメージしか入りません" }, ih01_backstab,     ih01_backstab_hidden },
    { { "보스 전리품", "ボスの戦利品" }, { "보스를 잡아도 희귀 아이템이 안 나온다", "ボスを倒してもレアアイテムが出ません" }, ih02_boss_loot,    ih02_boss_loot_hidden },
    { { "저격 탑", "スナイパータワー" }, { "저격 탑 사거리가 기본 탑과 같다", "スナイパータワーの射程が基本タワーと同じです" }, ih03_sniper_tower, ih03_sniper_tower_hidden },
    { { "사제의 치유", "プリーストの回復" }, { "사제의 기본 치유량이 너무 적다", "プリーストの基本回復量が少なすぎます" }, ih04_priest_heal,  ih04_priest_heal_hidden },
    { { "보상 우편", "報酬メール" }, { "골드 500을 보냈는데 500번 아이템이 왔다", "ゴールド500を送ったのに500番のアイテムが届きました" }, ih05_reward_mail, ih05_reward_mail_hidden },
    { { "마나 물약", "マナポーション" }, { "전사가 파티에 끼면 죽는다", "戦士がパーティーに入ると落ちます" }, ih06_mana_potion,  ih06_mana_potion_hidden },
    // 7부: 파일 입출력
    { { "패치된 가격표", "パッチ後の価格表" }, { "어떤 실행에서는 패치 전 가격이 나온다", "起動のしかたによってはパッチ前の価格が出ます" }, io01_patch_prices, io01_patch_prices_hidden },
    { { "모험 일지", "冒険日誌" }, { "방금 적은 일지가 일지 창에 안 보인다", "今書いた日誌が日誌ウィンドウに出ません" }, io02_quest_journal, io02_quest_journal_hidden },
    { { "점수판", "スコアボード" }, { "마지막 기록이 두 번 나온다", "最後の記録が2回表示されます" }, io03_high_scores,  io03_high_scores_hidden },
    { { "여관 주인 대사", "宿屋の主人のセリフ" }, { "첫 대사가 비고 마지막 대사가 빠진다", "最初のセリフが空で、最後のセリフが抜けます" }, io04_npc_dialogue, io04_npc_dialogue_hidden },
    { { "보상표", "報酬表" }, { "보상 목록이 통째로 비어 있다", "報酬リストがまるごと空です" }, io05_reward_table, io05_reward_table_hidden },
    { { "설정 파일", "設定ファイル" }, { "메모장으로 고친 첫 줄 설정만 무시된다", "メモ帳で直した1行目の設定だけが無視されます" }, io06_settings_bom, io06_settings_bom_hidden },
    { { "안전한 저장", "安全なセーブ" }, { "저장에 실패하면 이전 세이브까지 날아간다", "保存に失敗すると前のセーブまで消えます" }, io07_safe_save, io07_safe_save_hidden },
    // 8부: 직렬화 (JSON/XML)
    { { "펫 슬롯", "ペットスロット" }, { "펫이 없는데 소환 버튼이 켜진다", "ペットがいないのに召喚ボタンが有効になります" }, sz01_pet_slot,     sz01_pet_slot_hidden },
    { { "치명타 배율", "クリティカル倍率" }, { "배율 1.5가 1배로 들어간다", "倍率1.5が1倍で計算されます" }, sz02_crit_multiplier, sz02_crit_multiplier_hidden },
    { { "스킬 바", "スキルバー" }, { "스킬 바 순서가 뒤죽박죽이 된다", "スキルバーの順番がばらばらになります" }, sz03_skill_bar,    sz03_skill_bar_hidden },
    { { "닉네임 자르기", "ニックネームの切り詰め" }, { "어떤 한글 닉네임은 저장이 안 된다", "一部の日本語ニックネームが保存できません" }, sz04_nickname_cut, sz04_nickname_cut_hidden },
    { { "캐릭터 위치", "キャラクターの位置" }, { "불러올 때마다 캐릭터가 조금씩 움직인다", "読み込むたびにキャラクターが少しずつ動きます" }, sz05_float_position, sz05_float_position_hidden },
    { { "몬스터 도감", "モンスター図鑑" }, { "패치 뒤 몬스터 HP가 전부 0이다", "パッチ後、モンスターのHPがすべて0です" }, sz06_bestiary,     sz06_bestiary_hidden },
    { { "길드 목록", "ギルド一覧" }, { "어떤 길드 이름이 있으면 목록이 통째로 안 열린다", "ある名前のギルドがあると一覧がまるごと開けません" }, sz07_guild_list, sz07_guild_list_hidden },
    // 9부: 포인터 오용
    { { "랭킹 보드", "ランキングボード" }, { "랭킹 줄이 전부 깨져서 나온다", "ランキングの行がすべて化けて表示されます" }, pt01_ranking,      pt01_ranking_hidden },
    { { "현상금 사냥", "賞金稼ぎ" }, { "골드가 음수 수억이 된다", "ゴールドがマイナス数億になります" }, pt02_bounty,       pt02_bounty_hidden },
    { { "폭발 이펙트", "爆発エフェクト" }, { "이펙트가 끝날 때 멈춘다", "エフェクトが終わるときに止まります" }, pt03_particles,    pt03_particles_hidden },
    { { "전리품 패킷", "戦利品パケット" }, { "받은 아이템 개수가 엉망이다", "受け取ったアイテムの数がめちゃくちゃです" }, pt04_loot_packet,  pt04_loot_packet_hidden },
    { { "장비 공격력", "装備の攻撃力" }, { "맨손 파티원에서 죽는다", "素手のパーティーメンバーのところで落ちます" }, pt05_equipment,    pt05_equipment_hidden },
    { { "길드 해산", "ギルド解散" }, { "길드를 내려도 메모리가 남는다", "ギルドを解散してもメモリが残ります" }, pt06_guild,        pt06_guild_hidden },
    { { "분노 버프", "怒りバフ" }, { "분노가 끝나도 공격력이 원래대로 안 돌아온다", "怒りが切れても攻撃力が元に戻りません" }, pt07_rage_timer, pt07_rage_timer_hidden },
    // 9부 끝: 스택/힙 오버플로
    { { "미니맵", "ミニマップ" }, { "작은 지도인데도 미니맵을 열면 바로 죽는다", "小さいマップなのにミニマップを開くとすぐ落ちます" }, of01_minimap,  of01_minimap_hidden },
    { { "던전 생성기", "ダンジョン生成器" }, { "방이 많은 던전을 만들면 죽는다", "部屋の多いダンジョンを作ると落ちます" }, of02_dungeon_gen,  of02_dungeon_gen_hidden },
    { { "리플레이 녹화", "リプレイ録画" }, { "녹화를 끝내고 녹화기를 닫을 때 죽는다", "録画を終えて録画機を閉じるときに落ちます" }, of03_replay_buffer, of03_replay_buffer_hidden },
    { { "이름 패킷", "名前パケット" }, { "긴 이름이 잘리고 앞사람 이름이 붙는다", "長い名前が切れて、前の人の名前がくっつきます" }, of04_name_packet, of04_name_packet_hidden },
    { { "스킬 트리", "スキルツリー" }, { "스킬을 배웠더니 골드가 이상하게 바뀐다", "スキルを覚えたらゴールドがおかしく変わります" }, of05_skill_tree, of05_skill_tree_hidden },
    // 10부: 비동기와 동시성
    { { "맵 로딩", "マップ読み込み" }, { "비동기로 불러오는데 로딩이 하나씩 진행된다", "非同期で読み込んでいるのに、読み込みが1つずつ進みます" }, as01_loading,      as01_loading_hidden },
    { { "대장간 일괄 제작", "鍛冶屋の一括製作" }, { "주문 하나가 실패하면 대장간이 통째로 멈춘다", "注文が1つ失敗すると鍛冶屋がまるごと止まります" }, as02_crafting,    as02_crafting_hidden },
    { { "레이드 입장 대기", "レイド入場待ち" }, { "모두 준비했는데 공대장이 입장하지 않는다", "全員が準備したのにレイドリーダーが入場しません" }, as03_raid_ready,   as03_raid_ready_hidden },
    { { "낙찰 공지", "落札の告知" }, { "경매 창을 닫으면 낙찰가가 이상하게 공지된다", "オークション画面を閉じると落札価格がおかしく告知されます" }, as04_auction_notice, as04_auction_notice_hidden },
    { { "한정 판매", "限定販売" }, { "3개 한정인데 5개가 팔렸다", "3個限定なのに5個売れました" }, as05_limited_sale, as05_limited_sale_hidden },
    { { "매치메이킹", "マッチメイキング" }, { "대기열 인원이 모자라면 서버가 죽는다", "キューの人数が足りないとサーバーが落ちます" }, as06_matchmaking,  as06_matchmaking_hidden },
    { { "컷신 대화", "カットシーンの会話" }, { "컷신의 호감도 숫자가 이상하다", "カットシーンの好感度の数字がおかしいです" }, as07_cutscene,     as07_cutscene_hidden },
    // 11부: 멈춤 - 무한루프와 데드락
    { { "스킬 쿨다운", "スキルのクールダウン" }, { "파이어볼 쿨다운 계산에서 멈춘다", "ファイアボールのクールダウン計算で止まります" }, hg01_cooldown,     hg01_cooldown_hidden },
    { { "부활 카운트다운", "復活カウントダウン" }, { "부활 알림을 세다가 멈춘다", "復活通知を数えている途中で止まります" }, hg02_respawn,      hg02_respawn_hidden },
    { { "채팅 명령어", "チャットコマンド" }, { "명령어를 입력하면 멈춘다", "コマンドを入力すると止まります" }, hg03_chat_command, hg03_chat_command_hidden },
    { { "자동 이동 경로", "自動移動ルート" }, { "숲을 경로에 넣으면 멈춘다", "森をルートに入れると止まります" }, hg04_route,        hg04_route_hidden },
    { { "동시 거래", "同時トレード" }, { "두 사람이 서로 팔면 둘 다 멈춘다", "2人が互いに売ると2人とも止まります" }, hg05_trade,        hg05_trade_hidden },
    // 12부: 성능 - 틀리지 않지만 느리다
    { { "채팅 큐", "チャットキュー" }, { "메시지가 몰리면 서버가 굳는다", "メッセージが集中するとサーバーが固まります" }, pf01_chat_queue,   pf01_chat_queue_hidden },
    { { "장비 무게 HUD", "装備重量HUD" }, { "장비가 많으면 프레임이 떨어진다", "装備が多いとフレームレートが落ちます" }, pf02_loadout,      pf02_loadout_hidden },
    { { "전투 기록 내보내기", "戦闘記録のエクスポート" }, { "긴 레이드일수록 내보내기가 오래 걸린다", "長いレイドほどエクスポートに時間がかかります" }, pf03_combat_log, pf03_combat_log_hidden },
    { { "세이브 파일", "セーブファイル" }, { "큰 맵에서 저장이 몇 초씩 걸린다", "大きなマップでは保存に数秒かかります" }, pf04_save_log,     pf04_save_log_hidden },
    { { "전장의 안개", "戦場の霧" }, { "큰 맵에서만 프레임이 뚝 떨어진다", "大きなマップでだけフレームレートががくっと落ちます" }, pf05_fog_map,      pf05_fog_map_hidden },
    { { "어그로 검사", "ヘイト検査" }, { "몬스터가 많으면 서버 틱이 밀린다", "モンスターが多いとサーバーのティックが遅れます" }, pf06_aggro_scan,   pf06_aggro_scan_hidden },
};
// 챌린지: 기본 부를 모두 끝낸 뒤 푸는 문제. 힌트가 없고, 블라인드(B)는 종류도 밝히지 않는다.
// 채점 번호는 깊은 추적 C-n = 100 + n, 블라인드 B-n = 200 + n (grade-report.json의 id, BugHunt 네 판 공통)
struct ChallengeEntry
{
    const char* code;
    int gradeId;
    i18n::Text title;
    i18n::Text symptom;
    void (*run)();
    void (*hidden)();
};

constexpr ChallengeEntry kChallenges[] = {
    // 깊은 추적
    { "C-1", 101, { "챌린지 C-1", "チャレンジ C-1" }, { "낙찰자와 금화가 엉뚱하게 정산된다", "落札者と金貨がおかしく精算されます" }, chC1_auction_night, chC1_auction_night_hidden },
    { "C-2", 102, { "챌린지 C-2", "チャレンジ C-2" }, { "무피격 연속 처치 칭호를 받지 못한다", "ノーダメージ連続撃破の称号がもらえません" }, chC2_raid_night,    chC2_raid_night_hidden },
    { "C-3", 103, { "챌린지 C-3", "チャレンジ C-3" }, { "부활하고 나면 피해량이 줄어든다", "復活するとダメージが減ります" }, chC3_shrine,        chC3_shrine_hidden },
    // 블라인드
    { "B-1", 201, { "챌린지 B-1", "チャレンジ B-1" }, { "시장 진열대를 검사하다가 죽는다", "市場の陳列棚を検査している途中で落ちます" }, chB1, chB1_hidden },
    { "B-2", 202, { "챌린지 B-2", "チャレンジ B-2" }, { "공지 게시판의 작성자 이름이 깨진다", "お知らせ掲示板の作成者名が化けます" }, chB2, chB2_hidden },
    { "B-3", 203, { "챌린지 B-3", "チャレンジ B-3" }, { "넓은 지형에서 계산이 끝나지 않는다", "広い地形で計算が終わりません" }, chB3, chB3_hidden },
    { "B-4", 204, { "챌린지 B-4", "チャレンジ B-4" }, { "몬스터 체력 바가 이상하다", "モンスターのHPバーがおかしいです" }, chB4, chB4_hidden },
    { "B-5", 205, { "챌린지 B-5", "チャレンジ B-5" }, { "소환사가 부른 부하가 일부만 나온다", "召喚士が呼んだ手下が一部しか出てきません" }, chB5, chB5_hidden },
    { "B-6", 206, { "챌린지 B-6", "チャレンジ B-6" }, { "접속 기록 정리가 너무 오래 걸린다", "ログイン記録の整理に時間がかかりすぎます" }, chB6, chB6_hidden },
    { "B-7", 207, { "챌린지 B-7", "チャレンジ B-7" }, { "무료 배송 금액인데 배송비가 붙는다", "送料無料の金額なのに送料がかかります" }, chB7, chB7_hidden },
    { "B-8", 208, { "챌린지 B-8", "チャレンジ B-8" }, { "포탑이 경보에 제대로 반응하지 않는다", "砲塔が警報にきちんと反応しません" }, chB8, chB8_hidden },
};
constexpr int kLabCount = static_cast<int>(std::size(kLabs));

// 부마다 마지막 랩 번호와 메뉴 머리말
struct PartEntry
{
    int lastLab;
    i18n::Text header;
};

constexpr PartEntry kParts[] = {
    {  8, { "[1부] 호출 스택 입문 - 디버거로 실행하세요", "[第1部] 呼び出し履歴入門 - デバッガーで実行してください" } },
    { 20, { "[2부] 심화", "[第2部] 応用" } },
    { 32, { "[3부] 흐름 추적 - 크래시 없음. 중단점을 걸고 한 줄씩 따라가세요 (29~32: 연산자 실수)", "[第3部] 流れの追跡 - クラッシュなし。ブレークポイントを置いて1行ずつ追ってください (29~32: 演算子のミス)" } },
    { 39, { "[4부] 컨테이너 - 표준 컨테이너와 알고리즘의 함정", "[第4部] コンテナー - 標準コンテナーとアルゴリズムの落とし穴" } },
    { 48, { "[5부] C++ 언어 함정 - 컴파일러가 받아들이는 코드. 조사식으로 식을 나누고 경고를 읽으세요", "[第5部] C++ 言語の落とし穴 - コンパイラーが受け入れるコード。ウォッチで式を分け、警告を読んでください" } },
    { 54, { "[6부] 인터페이스와 상속 - 엉뚱한 함수가 불린다. Step Into로 실제 형식을 보세요", "[第6部] インターフェイスと継承 - 違う関数が呼ばれます。ステップインで実際の型を見てください" } },
    { 61, { "[7부] 파일 입출력 - 스트림 상태와 디스크의 실제 바이트", "[第7部] ファイル入出力 - ストリームの状態とディスク上の実際のバイト" } },
    { 68, { "[8부] 직렬화 (JSON/XML) - nlohmann/json, pugixml", "[第8部] シリアライズ (JSON/XML) - nlohmann/json, pugixml" } },
    { 80, { "[9부] 포인터 오용 - Debug, Release, ASan에서 각각 실행해 비교해 보세요 (76~80: 스택/힙 오버플로)", "[第9部] ポインターの誤用 - Debug、Release、ASan でそれぞれ実行して比べてください (76~80: スタック/ヒープのオーバーフロー)" } },
    { 87, { "[10부] 비동기와 동시성 - future, condition_variable, atomic, 스레드 수명, 코루틴", "[第10部] 非同期と並行性 - future、condition_variable、atomic、スレッドの寿命、コルーチン" } },
    { 92, { "[11부] 멈춤 - 무한루프와 데드락. 멈추면 디버거에서 모두 중단(break all)", "[第11部] 停止 - 無限ループとデッドロック。止まったらデバッガーで「すべて中断」(break all)" } },
    { 98, { "[12부] 성능 - 틀리지 않지만 느리다. Release + CPU 프로파일러", "[第12部] 性能 - 間違ってはいないが遅い。Release + CPU プロファイラー" } },
};
static_assert(i18n::allTranslated(kParts, [](const PartEntry& p) { return p.header; }), "kParts에 빈 번역이 있다");
static_assert(kParts[std::size(kParts) - 1].lastLab == static_cast<int>(std::size(kLabs)), "kParts와 kLabs의 랩 수가 다르다");

static_assert(i18n::allTranslated(kLabs, [](const LabEntry& e) { return e.title; }) && i18n::allTranslated(kLabs, [](const LabEntry& e) { return e.symptom; }),
              "kLabs에 빈 번역이 있다");
static_assert(i18n::allTranslated(kChallenges, [](const ChallengeEntry& e) { return e.title; }) && i18n::allTranslated(kChallenges, [](const ChallengeEntry& e) { return e.symptom; }),
              "kChallenges에 빈 번역이 있다");

constexpr int kChallengePart = 0;   // grade-report.json의 part. 챌린지는 0 (네 판 공통)

int partOf(int labNumber)
{
    for (int p = 0; p < static_cast<int>(std::size(kParts)); ++p)
    {
        if (labNumber <= kParts[p].lastLab)
            return p + 1;
    }
    return kChallengePart;
}

const char* buildName()
{
    return platform::buildName();
}

void printMenu()
{
    std::cout << i18n::L("=== BugHunt: C++ 버그 추적 실습 [", "=== BugHunt: C++ バグ追跡の実習 [") << buildName() << "] ===\n";
    int part = 0;
    for (int i = 0; i < kLabCount; ++i)
    {
        if (i == 0 || i == kParts[part - 1].lastLab)
            std::cout << '\n' << i18n::pick(kParts[part++].header) << '\n';
        std::cout << "  " << (i + 1 < 10 ? " " : "") << (i + 1) << ". "
                  << i18n::pick(kLabs[i].title) << "  - " << i18n::pick(kLabs[i].symptom) << '\n';
    }
    std::cout << i18n::L("\n[챌린지] 기본 부를 모두 끝낸 뒤에 푸세요. 힌트 없음 (README의 \"챌린지\")\n", "\n[チャレンジ] 基本の部をすべて終えてから解いてください。ヒントなし (README の「チャレンジ」)\n");
    for (const ChallengeEntry& c : kChallenges)
        std::cout << "  " << c.code << ". " << i18n::pick(c.symptom) << '\n';
    std::cout << i18n::L("  (명령줄 인자로 번호를 넘겨도 된다: BugHunt 3, BugHunt C-1)\n", "  (コマンドライン引数で番号を渡しても構いません: BugHunt 3, BugHunt C-1)\n");
    std::cout << i18n::L("  (채점: grade.bat, ./grade.sh 또는 BugHunt --grade. 언어: --lang ko|ja)\n", "  (採点: grade.bat、./grade.sh または BugHunt --grade。言語: --lang ja|ko)\n");
}

const ChallengeEntry* findChallenge(int gradeId)
{
    for (const ChallengeEntry& c : kChallenges)
    {
        if (c.gradeId == gradeId)
            return &c;
    }
    return nullptr;
}

// "7" → 7, "C-1" / "c1" / "101" → 101, "B-3" → 203. 알 수 없으면 0
int parseId(const std::string& text)
{
    if (text.empty())
        return 0;
    const char head = static_cast<char>(std::toupper(static_cast<unsigned char>(text[0])));
    if (head == 'C' || head == 'B')
    {
        const size_t digits = text.find_first_of("0123456789");
        if (digits == std::string::npos)
            return 0;
        const int id = (head == 'C' ? 100 : 200) + std::atoi(text.c_str() + digits);
        return findChallenge(id) ? id : 0;
    }
    const int id = std::atoi(text.c_str());
    return (id >= 1 && id <= kLabCount) || findChallenge(id) ? id : 0;
}

// 대상: "" = 전부, "labs" = 기본 랩만, "challenges" = 챌린지만, 그 밖에는 번호 하나
int grade(const std::string& what)
{
    const bool all = what.empty();
    const int only = all || what == "labs" || what == "challenges" ? 0 : parseId(what);

    std::vector<GradeTarget> targets;
    for (int i = 0; i < kLabCount; ++i)
    {
        const int id = i + 1;
        if (all || what == "labs" || only == id)
            targets.push_back({ id, std::to_string(id), i18n::pick(kLabs[i].title), partOf(id), kLabs[i].hidden != nullptr });
    }
    for (const ChallengeEntry& c : kChallenges)
    {
        if (all || what == "challenges" || only == c.gradeId)
            targets.push_back({ c.gradeId, c.code, i18n::pick(c.title), kChallengePart, c.hidden != nullptr });
    }
    if (targets.empty())
    {
        std::cout << "1 ~ " << kLabCount << i18n::L(" 사이의 번호, C-1 같은 챌린지 번호, labs, challenges 중 하나를 넣으세요.\n", " の番号、C-1 のようなチャレンジ番号、labs、challenges のどれかを入れてください。\n");
        return 1;
    }
    return runGrader(targets, kLabCount + static_cast<int>(std::size(kChallenges)), buildName());
}

int runLab(int id, bool hiddenCase)
{
    void (*run)() = nullptr;
    void (*hidden)() = nullptr;
    std::string heading;
    if (id >= 1 && id <= kLabCount)
    {
        const LabEntry& lab = kLabs[id - 1];
        heading = i18n::L("Lab ", "ラボ ") + std::to_string(id) + ": " + i18n::pick(lab.title);
        run = lab.run;
        hidden = lab.hidden;
    }
    else if (const ChallengeEntry* c = findChallenge(id))
    {
        heading = i18n::pick(c->title);
        run = c->run;
        hidden = c->hidden;
    }
    else
    {
        std::cout << "1 ~ " << kLabCount << i18n::L(" 사이의 번호나 C-1 같은 챌린지 번호를 입력하세요.\n", " の番号か、C-1 のようなチャレンジ番号を入力してください。\n");
        return 1;
    }
    if (hiddenCase)
    {
        if (hidden == nullptr)
            return kExitPass;
        std::cout << "\n--- " << heading << i18n::L(" (숨은 케이스) [", " (隠しケース) [") << buildName() << "] ---\n";
        hidden();
    }
    else
    {
        std::cout << "\n--- " << heading << " [" << buildName() << "] ---\n";
        run();
    }
    std::cout << i18n::L("--- 종료 ---\n", "--- 終了 ---\n");
    return g_failCount == 0 ? kExitPass : kExitFail;
}
}

int main(int argc, char** argv)
{
    platform::initConsole();

    // --lang ja|ko를 읽고 지운다 (환경 변수 BUGHUNT_LANG, 운영체제 언어 순으로 기본값). 나머지 인자로 아래를 처리한다.
    const std::vector<std::string> args = i18n::init(argc, argv);
    const int argCount = static_cast<int>(args.size());

    // 채점: BugHunt.exe --grade [번호 | C-1 | labs | challenges]
    if (argCount > 1 && args[1] == "--grade")
        return grade(argCount > 2 ? args[2] : "");

    // 채점기가 띄우는 자식 프로세스: BugHunt.exe --grade-child 채점번호 [--hidden]
    if (argCount > 2 && args[1] == "--grade-child")
    {
        prepareGradeChild();
        const bool hiddenCase = argCount > 3 && args[3] == "--hidden";
        return runLab(std::atoi(args[2].c_str()), hiddenCase);
    }

    std::string choice;
    if (argCount > 1)
    {
        choice = args[1];
    }
    else
    {
        printMenu();
        std::cout << i18n::L("\n실행할 랩 번호 (챌린지는 C-1, B-1처럼): ", "\n実行するラボ番号 (チャレンジは C-1、B-1 のように): ");
        std::cin >> choice;
    }

    return runLab(parseId(choice), false);
}
