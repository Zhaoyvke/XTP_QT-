#pragma once

//QWidge 尺寸定义宏
#define MyQLabelWideth 120
#define MyQLabelHeight 30
#define MyQLineEditHeight 30
#define MyQLineEditWidth 80
#define MyQComboBoxWidth 80
#define MyQComboBoxHeiht 30
#define MyQPushButtonWidth 80
#define MyQPushButtonHeight 30
#define MyQSpinBoxWidth 80
#define MyQSpinBoxHeight 30


//交易方向类型
#define Buy_MyTradeDirectionType '0'
#define Sell_MyTradeDirectionType '1'

typedef char MyTradeDirectionType;

//信息任务类型
#define Quit_MyInfoTaskType '0'
#define Message_MyInfoTaskType '1'
#define TradeApiConnected_MyInfoTaskType '2'
#define MarketDataApiConnected_MyInofoTaskType '2'

typedef char MyInfoTaskType;