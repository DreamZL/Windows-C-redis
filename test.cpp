#include "test.h"

#define  HSET "HSET"
#define  HMSET "HMSET"
#define  HGET "HGET"
#define  HGETALL "HGETALL"

class RedisConnect 
{
private:
  redisContext* redisCon; 
  redisReply* reply;
public:
  RedisConnect() : redisCon(nullptr), reply(nullptr) {}
  bool Init(const string & ip, int port)
  {
    if(nullptr != redisCon)
    {
      return false;
    }
    redisCon = redisConnect(ip.c_str(), port);  //连接redis服务器
    if(redisCon->err)
    {
      cerr << "error code: " << redisCon->err << ". " << redisCon->errstr << endl;
      return false;
    }
    return true;
  }
  
  bool InitWithTimeout(const string & ip, int port, int seconds )
  {
    if(nullptr != redisCon)
    {
      return false;
    }
    timeval tv;
    tv.tv_sec = seconds;
    tv.tv_usec = 0;
    redisCon = redisConnectWithTimeout(ip.c_str(), port, tv);
    if(redisCon->err)
    {
      cerr << "error code: " << redisCon->err << ". " << redisCon->errstr << endl;
      return false;
    }
    return true;
  }
  
  void freeReply()
  {
    if(nullptr != reply)
    {
      freeReplyObject(reply);
      reply = nullptr;
    }
  }
  
  ~RedisConnect()
  {
    freeReply();
    if(nullptr != redisCon)
    {
      redisFree(redisCon);
      redisCon = nullptr;
    }
  }
  
  template<class T, class... Args>
  bool HashSet(const string & command, T head, Args... rest)
  {
    stringstream ss;
    ss << command << " " << head << " ";
    return HashSetInner(ss, rest...);
  }
  
  template<class T, class... Args>
  bool HashGet(const string & command, T head, Args... rest)
  {
    stringstream ss;
    ss << command << " " << head << " ";
    return HashGetInner(ss, rest...);
  }
  
  template<class T>
  bool Set(const string & key, const T & value)
  {
    bool bRet = false;
    stringstream ss;
    ss << "SET " << key << " " << value;
    string s;
    getline(ss, s);
    return Set(s);
  }
  
  redisReply * Get(const string & key)
  {
    bool bRet = false;
    stringstream ss;
    ss << "GET " << key << " ";
    string s;
    getline(ss, s);
    freeReply();
    reply = (redisReply *)redisCommand(redisCon, s.c_str());
    return reply;
  }
  
  bool DelRedisKey(const string & key)
  {
    bool bRet = false;
    stringstream ss;
    ss << "DEL " << key << " ";
    string s;
    getline(ss, s);
    freeReply();
    reply = (redisReply *)redisCommand(redisCon, s.c_str());
    bRet = true;
    return bRet;
  }
  
private:
  //展开函数（展开参数包）
  template<class T, class... Args>
  bool HashSetInner(stringstream & ss, T head, Args... rest)
  {
    ss << head << " ";
    return HashSetInner(ss, rest...);
  }
  
  //递归终止函数
  bool HashSetInner(stringstream & ss)
  {
    bool bRet = false;
    string data;
    getline(ss, data);
    freeReply();
    reply = (redisReply *)redisCommand(redisCon, data.c_str());
    if(reply->type == REDIS_REPLY_ERROR || 
      (reply->type == REDIS_REPLY_STATUS && _stricmp(reply->str, "OK") != 0))
    {
      if(reply->str != nullptr)
      {
        cout << reply->str << endl;
      }
      cout << "Failed to excute " << __FUNCTION__ << endl << endl;
      return bRet;
    }
    bRet = true;
    return bRet;
  }
  
  //展开函数
  template<class T, class...Args>
  bool HashGetInner(stringstream & ss, T head, Args... rest)
  {
    ss << head << " ";
    return HashGetInner(ss, rest...);
  }
  
  //递归终止函数
  bool HashGetInner(stringstream & ss)
  {
    bool bRet = false;
    string data;
    getline(ss, data);
    freeReply();
    reply = (redisReply *)redisCommand(redisCon, data.c_str());
    if(reply->type == REDIS_REPLY_ERROR || 
      (reply->type == REDIS_REPLY_STATUS && _stricmp(reply->str, "OK") != 0))
    {
      if(reply->str != nullptr)
      {
        cout << reply->str << endl;
      }
      cout << "Failed to excute " << __FUNCTION__ << endl << endl;
      return bRet;
    }
    bRet = true;
    
    switch(reply->type)
    {
    case REDIS_REPLY_STRING:
    {
      cout << reply->str << endl;
      break;
    }
    case REDIS_REPLY_ARRAY:
    {
      for(size_t i = 0; i < reply->elements; ++i)
      {
        cout << reply->element[i]->str << endl;
      }
    }
    case REDIS_REPLY_INTEGER:
    {
      cout << reply->integer << endl;
      break;
    }
    default:
      break;
    }
    return bRet;
  }
  
  bool Set(string & data)
  {
    bool bRet = false;
    freeReply();
    reply = (redisReply *)redisCommand(redisCon, data.c_str());
    if(reply->type == REDIS_REPLY_ERROR || 
      (reply->type == REDIS_REPLY_STATUS && _stricmp(reply->str, "OK") != 0))
    {
      if(reply->str != nullptr)
      {
        cout << reply->str << endl;
      }
      cout << "Failed to excute " << __FUNCTION__ << endl << endl;
      return bRet;
    }
    bRet = true;
    return bRet;
  }
};


void main()
{
  RedisConnect r;
  bool b = r.InitWithTimeout("127.0.0.1", 6379, 5);
  if(!b)
  {
    cout << "Connect redis failed!" << endl;
  }
  
  r.Set("testtimes", 1);
  r.Set("float:pi", 3.14159265);
  r.Set("string", "hello world!");
  
  auto p = r.Get("string");
  if(p)
  {
    cout << p->str << endl;
  }
  
  r.HashSet(HMSET, "myhash", "name", "zhaolong", "age", 25);
  r.HashSet(HSET, "myhash", "email", "2745818@qq.com");
  
  r.HashGet(HGET, "myhash", "name");
  
}
