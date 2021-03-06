#ifndef SRC_BACKTEST_STRATEGY_H_
#define SRC_BACKTEST_STRATEGY_H_

#include <struct/market_snapshot.h>
#include <struct/strategy_status.h>
#include <util/time_controller.h>
#include <struct/order.h>
#include <struct/command.h>
#include <util/sender.hpp>
#include <util/dater.h>
#include <struct/exchange_info.h>
#include <struct/info_type.h>
#include <struct/order_status.h>
#include <util/history_worker.h>
#include <util/contract_worker.h>
#include <util/common_tools.h>
#include <core/base_strategy.h>
#include <libconfig.h++>
#include <unordered_map>

#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <memory>

class Strategy : public BaseStrategy {
 public:
  explicit Strategy(const libconfig::Setting & param_setting, std::unordered_map<std::string, std::vector<BaseStrategy*> >*ticker_strat_map, Sender<MarketSnapshot>* uisender, Sender<Order>* ordersender, HistoryWorker* hw, TimeController* tc, ContractWorker* cw, const std::string & mode = "real", std::ofstream* exchange_file = nullptr);
  ~Strategy();

  void Start() override;
  void Stop() override;

  // void Clear() override;
  void HandleCommand(const Command& shot) override;
  // void UpdateTicker() override;
 private:
  bool FillStratConfig(const libconfig::Setting& param_setting);
  void RunningSetup(std::unordered_map<std::string, std::vector<BaseStrategy*> >*ticker_strat_map, Sender<MarketSnapshot>* uisender, Sender<Order>* ordersender, const std::string & mode);
  void ClearPositionRecord();
  void DoOperationAfterUpdateData(const MarketSnapshot& shot) override;
  void DoOperationAfterUpdatePos(Order* o, const ExchangeInfo& info) override;
  void DoOperationAfterFilled(Order* o, const ExchangeInfo& info) override;
  void DoOperationAfterCancelled(Order* o) override;
  void ModerateOrders(const std::string & contract) override;

  void Init() override;
  bool Ready() override;
  void Pause() override;
  void Resume() override;
  void Run() override;
  void Train() override;
  void Flatting() override;

  void UpdateBuildPosTime();

  double OrderPrice(const std::string & contract, OrderSide::Enum side, bool control_price) override;

  OrderSide::Enum OpenLogicSide();
  bool OpenLogic();
  void CloseLogic();

  void Open(OrderSide::Enum side);
  bool Close(bool force_flat = false);

  void RecordSlip(const std::string & ticker, OrderSide::Enum side, bool is_close = false);
  void RecordPnl(Order* o, bool force_flat = false);

  void CalParams();
  std::tuple<double, double> CalMeanStd(const std::vector<double> & v, int head, int num);
  bool HitMean();

  double GetPairMid();

  void ForceFlat() override;

  bool Spread_Good();

  bool IsAlign();

  void UpdateBound(OrderSide::Enum side);
  void StopLossLogic();
  void HandleTestOrder(Order *o);

  char order_ref[MAX_ORDERREF_SIZE];
  std::string main_ticker;
  std::string hedge_ticker;
  int max_pos;
  double min_price_move;

  // std::unordered_map<std::string, std::vector<BaseStrategy*> >*tsm;
  int cancel_limit;
  std::unordered_map<std::string, double> mid_map;
  double up_diff;
  double down_diff;
  double range_width;
  double mean;
  std::vector<double> map_vector;
  int current_pos;
  double min_profit;
  int min_train_sample;
  double min_range;
  double increment;
  std::string mode;
  std::string date;
  double spread_threshold;
  int closed_size;
  double last_valid_mid;
  double stop_loss_up_line;
  double stop_loss_down_line;
  int max_loss_times;
  double stop_loss_times;
  double stop_loss_margin;
  int max_close_try;
  double current_spread;
  bool is_started;
  bool no_close_today;
  // int open_count;
  // int close_count;
  HistoryWorker* m_hw;
  int max_round;
  int close_round;
  int split_num;
  std::vector<double> param_v;
  int sample_head;
  int sample_tail;
  std::ofstream* exchange_file;
};

#endif  // SRC_BACKTEST_STRATEGY_H_
