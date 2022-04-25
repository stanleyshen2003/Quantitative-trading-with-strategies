#include <iostream>
#include <fstream>
#include <algorithm> 
using namespace std;
int Rs[245];
struct TX{
	int year,month,day,exdate;
	int price[285][3];
};

int abs(int a){
	if(a>0){
		return a;
	}
	return a*(-1);
}

int strategy1(TX* ptx){												//老師的策略1(當日買賣) 
	int r=(ptx->price[284][2]-ptx->price[0][2])*200;
	return r;
}

int strategy2(TX* ptx,double stoploss){								//老師的策略2(停利) 
	int cost =ptx->price[0][2];
	int slp =(int)(cost*(1-stoploss)+0.5);
	int n;
	int final_price;
	for(n=1;n<285;n++){
		final_price=ptx->price[n][2];
		if(ptx->price[n][2]<=slp){
			break; 
		}
	}
	return (final_price-cost)*200;
}

int strategy3(TX* ptx,double stoploss, double takeprofit){			//老師的策略3(停損+停利) 
	int cost =ptx->price[0][2];
	int slp =(int)(cost*(1-stoploss)+0.5);
	int tpp=(int)(cost*(1+takeprofit)+0.5);
	int n;
	int final_price;
	for(n=1;n<285;n++){
		final_price=ptx->price[n][2];
		if(ptx->price[n][2]<=slp){
			break; 
		}
		if(ptx->price[n][2]>=tpp){
			break;
		}
	}
	return (final_price-cost)*200;
}

int strategy4(TX* ptx){												//老師的策略4(簡化版葛蘭碧交易法則) 
	double MA5[285];
	double sum;
	int i,n,cost,position=0,r=0;
	for(n=4;n<285;n++){												//算五天前的平均 
		sum=0;
		for(i=n-4;i<=n;i++){
			sum+=ptx->price[i][2];
		}
		MA5[n]=sum/5;
	}
	for(int n=6;n<285;n++){											//買進的訊號 
		if(ptx->price[n-1][2]<=MA5[n-1] && ptx->price[n][2]>MA5[n] && MA5[n-2]>MA5[n-1] && MA5[n]>MA5[n-1]){
			if(position==0){										//空手 -> 買多一口 
				position=1;
				cost=ptx->price[n][2];
			}
			else if(position==-1){									//空方 -> 平倉 
				position=0;
				r+=(cost-ptx->price[n][2])*200;
			}
		}
		else if(ptx->price[n-1][2]>=MA5[n-1] && ptx->price[n][2]<MA5[n] && MA5[n-2]<MA5[n-1] && MA5[n]<MA5[n-1]){
			if(position==1){										//上面那行是賣出的訊號 
				position=0;											//有一口 -> 平倉 
				r+=(ptx->price[n][2]-cost)*200;
			}
			else if(position==0){									//空手 -> 空一口 
				position=-1;
				cost=ptx->price[n][2];
			}
		}
	}
	if(position==1){												//最後的賣掉+結算 
		r+=(ptx->price[284][2]-cost)*200;
	}
	else if(position==-1){											//把空的買回來 
		r+=(cost-ptx->price[284][2])*200;
	}
	return r;
}

int strategy5(TX* ptx,double stoploss){								//我新增的策略(葛蘭碧+停損+最佳化) 
	double MA5[285];
	double sum;
	int final_price,slp,i,n,cost,position=0,r=0;
	int buyflag=0,sellflag=0;
	for(n=4;n<285;n++){												//基本上跟策略4差不多 
		sum=0;
		for(i=n-4;i<=n;i++){
			sum+=ptx->price[i][2];
		}
		MA5[n]=sum/5;
	}
	for(int n=6;n<285;n++){
		if(ptx->price[n-1][2]<=MA5[n-1] && ptx->price[n][2]>MA5[n] && MA5[n-2]>MA5[n-1] && MA5[n]>MA5[n-1]){
			if(position==0){
				position=1;
				cost=ptx->price[n][2];
			}
			else if(position==-1){
				position=0;
				r+=(cost-ptx->price[n][2])*200;
			}
		}
		else if(ptx->price[n-1][2]>=MA5[n-1] && ptx->price[n][2]<MA5[n] && MA5[n-2]<MA5[n-1] && MA5[n]<MA5[n-1]){
			if(position==1){
				position=0;
				r+=(ptx->price[n][2]-cost)*200;
			}
			else if(position==0){
				position=-1;
				cost=ptx->price[n][2];
			}
		}
		slp =(int)(cost*(1-stoploss)+0.5);							//加入停損的部分 
		final_price=ptx->price[n][2];
		if(ptx->price[n][2]<=slp){
			break; 
		}
	}
	if(position==1){
		r+=(ptx->price[284][2]-cost)*200;
	}
	else if(position==-1){
		r+=(cost-ptx->price[284][2])*200;
	}
	return r;
}

int strategy7(TX* ptx){												//選每天最佳方案 
	int max=strategy1(ptx);											//strategy2是停損，當天的獲益不會>0 
	if(max<strategy3(ptx,0.01,0.02)){
		max=strategy3(ptx,0.01,0.02);
	}
	if(max<strategy4(ptx)){
		max=strategy4(ptx);
	}
	if(max<0){
		return 0;
	}
	return max;
	
}

int strategy8(TX* ptx){												//不管使用任何策略的最大獲利 
	int r=0;
	for(int i=1;i<=284;i++){										//每漲一次就加一次 
		if(ptx->price[i][2]-ptx->price[i-1][2]>=0){
			r+=(ptx->price[i][2]-ptx->price[i-1][2])*200;
		}
	}
	return r;
}

int strategy9(TX* ptx){												//最大虧損 
	int r=0;
	for(int i=1;i<=284;i++){										//每跌一次就虧一次 
		if(ptx->price[i][2]-ptx->price[i-1][2]<=0){
			r+=(ptx->price[i][2]-ptx->price[i-1][2])*200;
		}
	}
	return r;
}

int strategy10(TX* ptx){
	int r=0;
	for(int i=1;i<=284;i++){										//先加每一段 
		r+=abs(ptx->price[i][2]-ptx->price[i-1][2])*200;
	}
	for(int i=2;i<=284;i++){										//下一行的判斷是有沒有轉折點 
		if((ptx->price[i-1][2]-ptx->price[i-2][2])*(ptx->price[i-1][2]-ptx->price[i][2])>0){
			if(abs(ptx->price[i][2]-ptx->price[i-1][2])>abs(ptx->price[i-2][2]-ptx->price[i-1][2])){
				r-=abs(ptx->price[i-2][2]-ptx->price[i-1][2])*200;	//減掉差距小的那段 
			}
			else{
				r-=abs(ptx->price[i][2]-ptx->price[i-1][2])*200;
			}
		}
	}
	return r;
}

double beststop(TX* ptx,int day_num,int str){						//尋找最佳參數 
	int best=0;
	double besti;
	TX* reset;
	reset=ptx;
	double best7[2];
	for(double i=0.005;i<0.06;i+=0.0001){							//用迴圈一直改參數 
		int R=0;
		ptx=reset;
		switch(str){
			case 5:													//找策略5的 
				for(int a=0;a<day_num;a++){
					R+=strategy5(ptx,i);
					ptx++;
				}
				break;
			case 6:													//找策略2最佳化的 
				for(int a=0;a<day_num;a++){
					R+=strategy2(ptx,i);
					ptx++;
				}
				break;
		}
		if(R>best){
			best=R;
			besti=i;
		}
	}
	
	return besti;
}

int trading(TX* data, int day_num, int strategy_no){
	TX *ptx;
	int daycount;
	int R=0;
	ptx=data;
	double beststoploss;
	if(strategy_no==6 ||strategy_no==5){							//先把值存下來，避免每一天都跑一次beststop 
		beststoploss=beststop(data,day_num,strategy_no);
	}
	for(daycount=0;daycount<day_num;daycount++){					//一天一天跑 
		switch(strategy_no){										//分策略 
			case 1:													//老師的策略1 
				R+=strategy1(ptx);
				Rs[daycount]=strategy1(ptx);
				break;
			case 2:													//老師的策略2 
				R+=strategy2(ptx,0.01);
				Rs[daycount]=strategy2(ptx,beststoploss);
				break;
			case 6:													//策略2最佳化的獲利
				R+=strategy2(ptx,beststoploss);   
				Rs[daycount]=strategy2(ptx,beststoploss);
				break;
			case 3:													//老師的策略3 
				R+=strategy3(ptx,0.01,0.02);
				Rs[daycount]=strategy3(ptx,0.01,0.02);
				break;
			case 4:													//老師的策略4 
				R+=strategy4(ptx);
				Rs[daycount]=strategy4(ptx);
				break;
			case 5:													//我的策略(葛蘭碧 + 停損 + 最佳化參數) 
				R+=strategy5(ptx,beststoploss);
				Rs[daycount]=strategy5(ptx,beststoploss);
				break;
			case 7:													//我的策略(每天選擇最佳策略) 
				R+=strategy7(ptx);
				Rs[daycount]=strategy7(ptx);
				break;
			case 8:													//不做空前提下的最大獲利 
				R+=strategy8(ptx);
				Rs[daycount]=strategy8(ptx);
				break;
			case 9:
				R+=strategy9(ptx);									//不做空前提下的最大虧損 
				Rs[daycount]=strategy9(ptx);
				break;
			case 10:												//可做空的最大獲利 
				R+=strategy10(ptx);
				Rs[daycount]=strategy10(ptx);
				break;
		}
		ptx++;
	}
	return R;
}

int main(){
	int day_num,count=0;
	char c,str[101];
	int year,month,day,hour,minute,exdate,price;
	TX *data, *ptx;
	ifstream fin;
	fin.open("TXF_minute_2021_285.csv");
	fin.getline(str,100);
	while(fin>>year){												//算天數 
		fin>>c;
		fin>>month>>c;
		fin>>day>>c;
		fin>>hour>>c;
		fin>>minute>>c;
		fin>>exdate>>c;
		fin>>price;
		if(hour==13 && minute==29){
			count++;
		}
	}
	fin.close();
	day_num=count;
	data = new TX[day_num];
	ptx = data;
	fin.open("TXF_minute_2021_285.csv");
	fin.getline(str,100);
	int m=0;

	while(fin>>year){												//存資料 
		fin>>c;
		fin>>month>>c;
		fin>>day>>c;
		fin>>hour>>c;
		fin>>minute>>c;
		fin>>exdate>>c;
		fin>>price;
		if(m==0){
			ptx->year =year;
			ptx->month=month;
			ptx->day=day;
			ptx->exdate=exdate;
		}
		ptx->price[m][0]=hour;
		ptx->price[m][1]=minute;
		ptx->price[m][2]=price;
		m++;
		if(hour==13 && minute==29){
			m=0;
			ptx++;
		}	
	}
	fin.close();
	cout<<endl;
	cout<<"策略1(buy and hold)"<<endl;
	cout<<"總共獲利 : "<< trading(data,day_num,1)<<endl;
	sort(Rs,Rs+245);
	cout<<"單日最高獲利 : "<<Rs[244]<<endl;
	cout<<"單日最高虧損 : "<<Rs[0]<<endl;
	cout<<endl; 
	cout<<"----------------------------"<<endl;
	cout<<endl;
	cout<<"策略2(B&H+停損)"<<endl;
	cout<<"總共獲利 : "<< trading(data,day_num,2)<<endl;
	sort(Rs,Rs+245);
	cout<<"單日最高獲利 : "<<Rs[244]<<endl;
	cout<<"單日最高虧損 : "<<Rs[0]<<endl;
	cout<<endl;
	cout<<"最佳化的stoploss參數: "<< beststop(data,day_num,6)<<endl;
	cout<<"最佳化後可獲利 "<< trading(data,day_num,6)<<endl;
	sort(Rs,Rs+245);
	cout<<"最佳化後單日最高獲利 : "<<Rs[244]<<endl;
	cout<<"最佳化後單日最高虧損 : "<<Rs[0]<<endl;
	cout<<endl;
	cout<<"----------------------------"<<endl;
	cout<<endl;
	cout<<"策略3(B&H+停損+停利)"<<endl;
	cout<<"總共獲利 : "<< trading(data,day_num,3)<<endl;
	sort(Rs,Rs+245);
	cout<<"單日最高獲利 : "<<Rs[244]<<endl;
	cout<<"單日最高虧損 : "<<Rs[0]<<endl;
	cout<<endl;
	cout<<"----------------------------"<<endl;
	cout<<endl;
	cout<<"策略4(簡化版葛蘭碧交易法則)"<<endl;
	cout<<"總共獲利 : "<< trading(data,day_num,4)<<endl;
	sort(Rs,Rs+245);
	cout<<"單日最高獲利 : "<<Rs[244]<<endl;
	cout<<"單日最高虧損 : "<<Rs[0]<<endl;
	cout<<endl;
	cout<<"----------------------------"<<endl;
	cout<<endl;
	cout<<"策略5(簡化版葛蘭碧交易法則+最佳停損)"<<endl;
	cout<<"總共獲利 : "<< trading(data,day_num,5)<<endl;
	cout<<"最佳化的stoploss參數: "<<beststop(data,day_num,5)<<endl;
	sort(Rs,Rs+245);
	cout<<"最佳化後單日最高獲利 : "<<Rs[244]<<endl;
	cout<<"最佳化後單日最高虧損 : "<<Rs[0]<<endl;
	cout<<endl;
	cout<<"----------------------------"<<endl;
	cout<<endl;
	cout<<"策略6(每天選最佳獲利的策略)"<<endl;
	cout<<"總共獲利 : "<<trading(data,day_num,7)<<endl;
	sort(Rs,Rs+245);
	cout<<"單日最高獲利 : "<<Rs[244]<<endl;
	cout<<"單日最高虧損 : "<<Rs[0]<<endl;
	cout<<endl;
	cout<<"----------------------------"<<endl;
	cout<<endl;
	cout<<"極限的獲利(不考慮做空)"<<endl;
	cout<<"總共獲利 : "<<trading(data,day_num,8)<<endl;
	sort(Rs,Rs+245);
	cout<<"單日最高獲利 : "<<Rs[244]<<endl;
	cout<<"單日最低獲利 : "<<Rs[0]<<endl;
	cout<<endl;
	cout<<"----------------------------"<<endl;
	cout<<endl;
	cout<<"極限的虧損(不考慮做空)"<<endl;
	cout<<"總共虧損 : "<<trading(data,day_num,9)<<endl;
	sort(Rs,Rs+245);
	cout<<"單日最低虧損 : "<<Rs[244]<<endl;
	cout<<"單日最高虧損 : "<<Rs[0]<<endl;
	cout<<endl;
	cout<<"----------------------------"<<endl;
	cout<<endl;
	cout<<"極限的獲利(考慮做空)"<<endl;
	cout<<"總共獲利 : "<<trading(data,day_num,10)<<endl;
	sort(Rs,Rs+245);
	cout<<"單日最高獲利 : "<<Rs[244]<<endl;
	cout<<"單日最小獲利 : "<<Rs[0]<<endl;
	return 0;
}
