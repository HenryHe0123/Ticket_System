### Ticket System debug 文档

- [x] basic_test2


query_ticket 一直输出0，buy_ticket 从一段时间后开始出错

输出调试发现不是没有找到stop信息，而是没有找到ticket 

同时发现找ticket过程中主要是date不匹配，重构了Date的加减法并去掉了6-8月的限制

检查需求文档时发现add train时-o可能省略，已补充

检查train信息时发现核心问题：误解了begin&endDate变量含义（反思：**取名要清晰！**）

重构：release_train, query_train，query_ticket，buy_ticket

再调试：发现release_train时 seats_map BPT insert internal error

仔细检查发现在BPT调用二分时index的比较符号不合理（>=不等价于>加=）已修复

再次调试：query_ticket 仍一直输出0，buy_ticket调用index错误，query_order日期有误，query_train金额有误

输出调试与检查发现buy_ticket忘记检查d是否在售票时间范围内，已修复；忘记更新order的日期（导致query_order错误），已修复；query_train金额理解有误，已更正；query_ticket在双指针匹配stop时比较符号不合理，已修复（反思：**比较符号要合理！**）

默认输出query_tranfer为0，pass！

- [x] basic_test4（2）

buy_ticket偶尔出错，可以购买的火车票被加入queue，以及query_order也偶尔出错

检查发现refund_ticket在检查候补订单时混淆了火车票区间（误用了退票订单的区间），已修复

pass！

- [x] basic_test3 （5）

query_transfer出错，换乘站错误，第二辆火车id错误，且输出默认price和time

检查发现换乘站错写成终点站，已修复；再调试，安全检查报错 query_transfer chaos2: best transfer found but wrong

输出调试发现火车id和换成站已正确，但dayAfterStart错误

发现换乘日期未必相邻（有发售区间限制），在transfer中添加wait信息，修复dayAfterStart

pass！

-------------------------------------------------------------------------------------------------------------------------------

TLE问题：

- [x] 增加缓存


测试发现 query_ticket 是导致TLE的罪魁祸首

主要耗时在于stop_multimap.find()

主要原因不在于BPT，在于存了太多重复的stop

- [x] 重构Stop类
- [x] 重构release_train
- [x] 重构query_ticket
- [x] 重构query_transfer

快快快快快快快快快快！

FINISHED !!!
