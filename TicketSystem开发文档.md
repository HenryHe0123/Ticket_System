## Ticket System 开发文档

#### 程序功能概述

1. 基础指令：清楚所有数据、退出程序
2. 账户系统：新建账户、登录、登出、查询资料、修改资料
3. 火车系统：添加、删除、发布、查询火车、查询车票、查询换乘
4. 火车交互系统：购买车票、查询订单、退订

#### 主题逻辑说明

##### User System

略

##### Train System

```c++
class Train {
	my::string<20> trainID;
    int stationNum = 2; //2 ~ N
    my::string<30> stations[N];
    int seat = 0;
    int prices[N]{0}; //stationNum - 1
    Time startTime; //for every day during Date begin to end!
    int travelTimes[N]{0}, stopoverTimes[N]{0}; //stationNum - 1/2
    Date beginDate, endDate; //saleDate
    char type = 0;
} //外部类
my::BPT<ustring, Train> train_map; //未发布火车信息
my::BPT<ustring, Train> released_trains;//已发布火车信息
//不包含座位信息
struct Seat; //一个记录座位剩余的int数组remain 
struct Index; //作为查询火车其他信息的索引(火车id，出发日期)
struct Stop; //存储火车停靠站信息(id，站台index，到达和离开时间，列车出发时间)
my::BPT<Index, Seat> seats_map; 
my::multiBPT<sstring, Stop> stop_multimap;
//只记录已发布火车座位信息
struct Ticket; //id，单价，剩余最大座位，到达和离开时间
struct Order {
    ustring username;
    Index index;
    int time = 0, status = 1, price = 0, num = 0; //price指单价，num指票数
    //status: 1-success, 0-pending, -1-refunded
    sstring from, to;
    int l = 0, r = 0; //index of from and right
    Date_Time start, end;
} //按time排序
my::multiBPT<ustring, Order> order_u; //记录每个用户的所有订单
my::multiBPT<Index, Order> pending_order; //记录候补队列
```

