extern "C" __declspec(dllexport) int TdxInit();
extern "C" __declspec(dllexport) int TdxExit();
extern "C" __declspec(dllexport) int TdxLogin(char *server,int port,char *Account,char *JyPass,char *TxPass,char *ErrInfo);
extern "C" __declspec(dllexport) int TdxLogout();
extern "C" __declspec(dllexport) int TdxSendOrder(int TradeType,int TradeMode,char *GdCode,char *StkCode,double Price,int volume,char *Result,char *ErrInfo);
extern "C" __declspec(dllexport) int TdxQueryData(int QueryMode,char *Result,char *ErrInfo);
extern "C" __declspec(dllexport) int TdxGetQuote(char* StockCode, char* Result, char* ErrInfo);
extern "C" __declspec(dllexport) int TdxQueryHistory(int Category, char* StartDate, char* EndDate, char* Result, char* ErrInfo);
extern "C" __declspec(dllexport) int TdxCancelOrder(char *TradeID,int Mark,char *Result,char *ErrInfo);


