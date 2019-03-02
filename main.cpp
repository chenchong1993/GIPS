#include <iostream>
#include <stdio.h>
#include <hiredis/hiredis.h>
#include "src/rtklib.h"
#include "gips.h"
#include <iomanip>
#include <sys/time.h>
using namespace std;

/*
 *
 */
void pushDataRedis(rtksvr_t rtksvr,redisContext* c,string StationName)
{
    string temp;
    string obsData = "";
    double time = rtksvr.rtcm[0].obs.data[0].time.time+rtksvr.rtcm[0].obs.data[0].time.sec;
    for(int i = 0;i<rtksvr.rtcm[0].obs.n;i++)
    {
        std::string prn = std::to_string(int(rtksvr.rtcm[0].obs.data[i].sat));
        std::string PL1 = std::to_string(rtksvr.rtcm[0].obs.data[i].P[0]);
        std::string PL2 = std::to_string(rtksvr.rtcm[0].obs.data[i].P[1]);
        std::string PL3 = std::to_string(rtksvr.rtcm[0].obs.data[i].P[2]);
        std::string PL4 = "0.00";//std::to_string(rtksvr.rtcm[0].obs.data[i].P[0]);
        std::string PL5 = "0.00";//std::to_string(rtksvr.rtcm[0].obs.data[i].P[0]);
        std::string LL1 = std::to_string(rtksvr.rtcm[0].obs.data[i].L[0]);
        std::string LL2 = std::to_string(rtksvr.rtcm[0].obs.data[i].L[1]);
        std::string LL3 = std::to_string(rtksvr.rtcm[0].obs.data[i].L[2]);
        std::string LL4 = "0.00";//std::to_string(rtksvr.rtcm[0].obs.data[i].P[0]);
        std::string LL5 = "0.00";//std::to_string(rtksvr.rtcm[0].obs.data[i].P[0]);
        std::string DL1 = std::to_string(rtksvr.rtcm[0].obs.data[i].D[0]);
        std::string DL2 = std::to_string(rtksvr.rtcm[0].obs.data[i].D[1]);
        std::string DL3 = std::to_string(rtksvr.rtcm[0].obs.data[i].D[2]);
        std::string DL4 = "0.00";//std::to_string(rtksvr.rtcm[0].obs.data[i].P[0]);
        std::string DL5 = "0.00";//std::to_string(rtksvr.rtcm[0].obs.data[i].P[0]);
        std::string SL1 = std::to_string(rtksvr.rtcm[0].obs.data[i].SNR[0]);
        std::string SL2 = std::to_string(rtksvr.rtcm[0].obs.data[i].SNR[1]);
        std::string SL3 = std::to_string(rtksvr.rtcm[0].obs.data[i].SNR[2]);
        std::string SL4 = "0.00";//std::to_string(rtksvr.rtcm[0].obs.data[i].P[0]);
        std::string SL5 = "0.00";//std::to_string(rtksvr.rtcm[0].obs.data[i].P[0]);
        string temp = "\""+prn+"\":{\"P\":{\"L1\":"+PL1+",\"L2\":"+PL2+",\"L3\":"+PL3+",\"L4\":"+PL4+",\"L5\":"+PL5+"},\"L\":{\"L1\":"+LL1+",\"L2\":"+LL2+",\"L3\":"+LL3+",\"L4\":"+LL4+",\"L5\":"+LL5+"},\"D\":{\"L1\":"+DL1+",\"L2\":"+DL2+",\"L3\":"+DL3+",\"L4\":"+DL4+",\"L5\":"+DL5+"},\"S\":{\"L1\":"+SL1+",\"L2\":"+SL2+",\"L3\":"+SL3+",\"L4\":"+SL4+",\"L5\":"+SL5+"}},";
        if(temp!=""){
        obsData.append(temp);
        }
    }
    obsData = obsData.substr(0, obsData.length() - 1);
    obsData = "{"+obsData+"}";
    char * charObsData=(char*)obsData.data();
    char * charStationName=(char*)StationName.data();
    redisReply* loadDate = (redisReply*)redisCommand(c,"set %s:%f %s ex 20 ",charStationName,time,charObsData);
    charObsData = NULL;
    charStationName = NULL;
}

int main()
{
    rtkStr rtkS;
	rtkS.showEvent();
	rtkS.SvrStart();
	//io.run();
/*
 *以下是链接数据库的配置。
 */
    redisContext* c = redisConnect("121.28.103.199", 5622);
    if (c->err) {    /* Error flags, 0 when there is no error */
        printf("ERRO NOT Connect Redis: %s\n", c->errstr);
        exit(1);
    }
    else{
        printf("Connect Redis！\n");
    }

    redisReply* reply = (redisReply*)redisCommand(c, "AUTH %s", "CASMadmin2019");
    if (reply->type == REDIS_REPLY_ERROR) {
        printf("ERRO NOT AUTH Redis\n");
    }
    else{
        printf("AUTH Redis！\n");
    }
    freeReplyObject(reply);

        //强制使用小数,防止使用科学计数法
    cout << fixed;
    //控制显示的精度，控制小数点后面的位数
    cout << setprecision(7);

	while (true)
	{
		rtkS.TimerTimer();
		if(rtksvr.rtcm[0].obs.n>0)
		{

            pushDataRedis(rtksvr,c,"STATION01");
        }
        //cout<<"1111"<<endl;

        //*p = NULL;
		cout<< rtksvr.rtk.sol.rr[1] <<endl;
		//cout<< rtksvr.rtk.sol.rr[2] <<endl;

		//redisReply* loadDate = (redisReply*)redisCommand(c, "set x %f",rtksvr.rtk.sol.rr[0]);
       /*
        struct  timeval    tv;
        struct  timezone   tz;
        gettimeofday(&tv,&tz);
        cout<<"nowTime"<<tv.tv_sec<<"."<<tv.tv_usec<<endl;

		redisReply* loadDate = (redisReply*)redisCommand(c,
		"mset %f:L1:P:%d %f",,);

		struct  timeval    tv_end;
        struct  timezone   tz_end;
        gettimeofday(&tv_end,&tz_end);
        cout<<"nowTime"<<tv_end.tv_sec<<"."<<tv_end.tv_usec<<endl;
        cout<<"nowTime"<<tv_end.tv_sec-tv.tv_sec<<"."<<tv_end.tv_usec-tv.tv_usec<<endl;
*/
		sleepms(1000);
	}


    return 0;
}
/*
void pushDataToRedis(double time,string L,string p,string l,string snr,)
{

}
*/

