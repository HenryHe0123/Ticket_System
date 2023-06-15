## Ticket System 开发文档

#### 程序功能概述

1. 基础指令：清楚所有数据、退出程序
2. 账户系统：新建账户、登录、登出、查询资料、修改资料
3. 火车系统：添加、删除、发布、查询火车、查询车票、查询换乘
4. 火车交互系统：购买车票、查询订单、退订

#### 主题结构说明

##### User System

```c++
class UserSystem {
    using ustring = my::string<20>;
    my::BPT<ustring, User> user_map;
    sjtu::map<ustring, int> user_login; //store privilege in int
};
```

##### Train System

```c++
class Train {
	my::string<20> trainID;
    int stationNum = 2; //2 ~ N
    my::string<30> stations[N];
    int seat = 0; //总计座位
    int prices[N]{0}; //stationNum - 1
    Time startTime; //for every day during Date begin to end!
    int travelTimes[N]{0}, stopoverTimes[N]{0}; //stationNum - 1/2
    Date beginDate, endDate; //saleDate
    char type = 0;
} //外部类
class TrainSystem {
    using ustring = my::string<20>;
    using sstring = my::string<30>;
private:
my::BPT<ustring, Train> train_map; //未发布火车信息
my::BPT<ustring, Train> released_trains;//已发布火车信息
//不包含座位信息
struct Seat; //一个记录座位剩余的int数组remain 
struct Index; //作为查询火车其他信息的索引(火车id，出发日期)
struct Stop; //存储火车停靠站信息(id，站台index，到达和离开时间(第一天)，列车出发区间)
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
    int l = 0, r = 0; //index of from and to
    Date_Time start, end;
} //按time(stamp)排序
my::multiBPT<ustring, Order> order_u; //记录每个用户的所有订单
my::multiBPT<Index, Order> pending_order; //记录候补队列
struct transferInfo; //火车id，抵达时间，花费时间，价格（用于transfer查询中的匹配）
struct Transfer; //id1，id2，总时间，总价格，换乘站，换乘等待时间
};
```

#### 关键函数逻辑说明

##### query_ticket

查询起始和终点站的所有火车停靠信息（按火车id递增），用双指针匹配对应火车：

先检查火车id是否对应以及站点次序是否正确，然后确定火车出发时间（检查是否在发售时间内），获取ticket加入vector。最后排序输出。

##### query_transfer

首先查询起始和终点站的所有火车停靠信息，遍历经过起始站的所有火车的后续站，并用hash-map存储信息（key: 站名，val:  vector<火车id，抵达时间，花费时间，价格>）。

再遍历经过终点站的所有火车的前序站，检查是否在hash-map中存在，若存在则读取vector（的指针），检查每辆火车的时间是否合适，并更新最优解。
