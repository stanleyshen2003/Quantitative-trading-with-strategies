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

int strategy1(TX* ptx){												//�Ѯv������1(���R��) 
	int r=(ptx->price[284][2]-ptx->price[0][2])*200;
	return r;
}

int strategy2(TX* ptx,double stoploss){								//�Ѯv������2(���Q) 
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

int strategy3(TX* ptx,double stoploss, double takeprofit){			//�Ѯv������3(���l+���Q) 
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

int strategy4(TX* ptx){												//�Ѯv������4(²�ƪ������ѥ���k�h) 
	double MA5[285];
	double sum;
	int i,n,cost,position=0,r=0;
	for(n=4;n<285;n++){												//�⤭�ѫe������ 
		sum=0;
		for(i=n-4;i<=n;i++){
			sum+=ptx->price[i][2];
		}
		MA5[n]=sum/5;
	}
	for(int n=6;n<285;n++){											//�R�i���T�� 
		if(ptx->price[n-1][2]<=MA5[n-1] && ptx->price[n][2]>MA5[n] && MA5[n-2]>MA5[n-1] && MA5[n]>MA5[n-1]){
			if(position==0){										//�Ť� -> �R�h�@�f 
				position=1;
				cost=ptx->price[n][2];
			}
			else if(position==-1){									//�Ť� -> ���� 
				position=0;
				r+=(cost-ptx->price[n][2])*200;
			}
		}
		else if(ptx->price[n-1][2]>=MA5[n-1] && ptx->price[n][2]<MA5[n] && MA5[n-2]<MA5[n-1] && MA5[n]<MA5[n-1]){
			if(position==1){										//�W������O��X���T�� 
				position=0;											//���@�f -> ���� 
				r+=(ptx->price[n][2]-cost)*200;
			}
			else if(position==0){									//�Ť� -> �Ť@�f 
				position=-1;
				cost=ptx->price[n][2];
			}
		}
	}
	if(position==1){												//�̫᪺�汼+���� 
		r+=(ptx->price[284][2]-cost)*200;
	}
	else if(position==-1){											//��Ū��R�^�� 
		r+=(cost-ptx->price[284][2])*200;
	}
	return r;
}

int strategy5(TX* ptx,double stoploss){								//�ڷs�W������(������+���l+�̨Τ�) 
	double MA5[285];
	double sum;
	int final_price,slp,i,n,cost,position=0,r=0;
	int buyflag=0,sellflag=0;
	for(n=4;n<285;n++){												//�򥻤W�򵦲�4�t���h 
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
		slp =(int)(cost*(1-stoploss)+0.5);							//�[�J���l������ 
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

int strategy7(TX* ptx){												//��C�ѳ̨Τ�� 
	int max=strategy1(ptx);											//strategy2�O���l�A��Ѫ���q���|>0 
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

int strategy8(TX* ptx){												//���ިϥΥ��󵦲����̤j��Q 
	int r=0;
	for(int i=1;i<=284;i++){										//�C���@���N�[�@�� 
		if(ptx->price[i][2]-ptx->price[i-1][2]>=0){
			r+=(ptx->price[i][2]-ptx->price[i-1][2])*200;
		}
	}
	return r;
}

int strategy9(TX* ptx){												//�̤j���l 
	int r=0;
	for(int i=1;i<=284;i++){										//�C�^�@���N���@�� 
		if(ptx->price[i][2]-ptx->price[i-1][2]<=0){
			r+=(ptx->price[i][2]-ptx->price[i-1][2])*200;
		}
	}
	return r;
}

int strategy10(TX* ptx){
	int r=0;
	for(int i=1;i<=284;i++){										//���[�C�@�q 
		r+=abs(ptx->price[i][2]-ptx->price[i-1][2])*200;
	}
	for(int i=2;i<=284;i++){										//�U�@�檺�P�_�O���S������I 
		if((ptx->price[i-1][2]-ptx->price[i-2][2])*(ptx->price[i-1][2]-ptx->price[i][2])>0){
			if(abs(ptx->price[i][2]-ptx->price[i-1][2])>abs(ptx->price[i-2][2]-ptx->price[i-1][2])){
				r-=abs(ptx->price[i-2][2]-ptx->price[i-1][2])*200;	//��t�Z�p�����q 
			}
			else{
				r-=abs(ptx->price[i][2]-ptx->price[i-1][2])*200;
			}
		}
	}
	return r;
}

double beststop(TX* ptx,int day_num,int str){						//�M��̨ΰѼ� 
	int best=0;
	double besti;
	TX* reset;
	reset=ptx;
	double best7[2];
	for(double i=0.005;i<0.06;i+=0.0001){							//�ΰj��@����Ѽ� 
		int R=0;
		ptx=reset;
		switch(str){
			case 5:													//�䵦��5�� 
				for(int a=0;a<day_num;a++){
					R+=strategy5(ptx,i);
					ptx++;
				}
				break;
			case 6:													//�䵦��2�̨Τƪ� 
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
	if(strategy_no==6 ||strategy_no==5){							//����Ȧs�U�ӡA�קK�C�@�ѳ��]�@��beststop 
		beststoploss=beststop(data,day_num,strategy_no);
	}
	for(daycount=0;daycount<day_num;daycount++){					//�@�Ѥ@�Ѷ] 
		switch(strategy_no){										//������ 
			case 1:													//�Ѯv������1 
				R+=strategy1(ptx);
				Rs[daycount]=strategy1(ptx);
				break;
			case 2:													//�Ѯv������2 
				R+=strategy2(ptx,0.01);
				Rs[daycount]=strategy2(ptx,beststoploss);
				break;
			case 6:													//����2�̨Τƪ���Q
				R+=strategy2(ptx,beststoploss);   
				Rs[daycount]=strategy2(ptx,beststoploss);
				break;
			case 3:													//�Ѯv������3 
				R+=strategy3(ptx,0.01,0.02);
				Rs[daycount]=strategy3(ptx,0.01,0.02);
				break;
			case 4:													//�Ѯv������4 
				R+=strategy4(ptx);
				Rs[daycount]=strategy4(ptx);
				break;
			case 5:													//�ڪ�����(������ + ���l + �̨ΤưѼ�) 
				R+=strategy5(ptx,beststoploss);
				Rs[daycount]=strategy5(ptx,beststoploss);
				break;
			case 7:													//�ڪ�����(�C�ѿ�̨ܳε���) 
				R+=strategy7(ptx);
				Rs[daycount]=strategy7(ptx);
				break;
			case 8:													//�����ūe���U���̤j��Q 
				R+=strategy8(ptx);
				Rs[daycount]=strategy8(ptx);
				break;
			case 9:
				R+=strategy9(ptx);									//�����ūe���U���̤j���l 
				Rs[daycount]=strategy9(ptx);
				break;
			case 10:												//�i���Ū��̤j��Q 
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
	while(fin>>year){												//��Ѽ� 
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

	while(fin>>year){												//�s��� 
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
	cout<<"����1(buy and hold)"<<endl;
	cout<<"�`�@��Q : "<< trading(data,day_num,1)<<endl;
	sort(Rs,Rs+245);
	cout<<"���̰���Q : "<<Rs[244]<<endl;
	cout<<"���̰����l : "<<Rs[0]<<endl;
	cout<<endl; 
	cout<<"----------------------------"<<endl;
	cout<<endl;
	cout<<"����2(B&H+���l)"<<endl;
	cout<<"�`�@��Q : "<< trading(data,day_num,2)<<endl;
	sort(Rs,Rs+245);
	cout<<"���̰���Q : "<<Rs[244]<<endl;
	cout<<"���̰����l : "<<Rs[0]<<endl;
	cout<<endl;
	cout<<"�̨Τƪ�stoploss�Ѽ�: "<< beststop(data,day_num,6)<<endl;
	cout<<"�̨Τƫ�i��Q "<< trading(data,day_num,6)<<endl;
	sort(Rs,Rs+245);
	cout<<"�̨Τƫ���̰���Q : "<<Rs[244]<<endl;
	cout<<"�̨Τƫ���̰����l : "<<Rs[0]<<endl;
	cout<<endl;
	cout<<"----------------------------"<<endl;
	cout<<endl;
	cout<<"����3(B&H+���l+���Q)"<<endl;
	cout<<"�`�@��Q : "<< trading(data,day_num,3)<<endl;
	sort(Rs,Rs+245);
	cout<<"���̰���Q : "<<Rs[244]<<endl;
	cout<<"���̰����l : "<<Rs[0]<<endl;
	cout<<endl;
	cout<<"----------------------------"<<endl;
	cout<<endl;
	cout<<"����4(²�ƪ������ѥ���k�h)"<<endl;
	cout<<"�`�@��Q : "<< trading(data,day_num,4)<<endl;
	sort(Rs,Rs+245);
	cout<<"���̰���Q : "<<Rs[244]<<endl;
	cout<<"���̰����l : "<<Rs[0]<<endl;
	cout<<endl;
	cout<<"----------------------------"<<endl;
	cout<<endl;
	cout<<"����5(²�ƪ������ѥ���k�h+�̨ΰ��l)"<<endl;
	cout<<"�`�@��Q : "<< trading(data,day_num,5)<<endl;
	cout<<"�̨Τƪ�stoploss�Ѽ�: "<<beststop(data,day_num,5)<<endl;
	sort(Rs,Rs+245);
	cout<<"�̨Τƫ���̰���Q : "<<Rs[244]<<endl;
	cout<<"�̨Τƫ���̰����l : "<<Rs[0]<<endl;
	cout<<endl;
	cout<<"----------------------------"<<endl;
	cout<<endl;
	cout<<"����6(�C�ѿ�̨���Q������)"<<endl;
	cout<<"�`�@��Q : "<<trading(data,day_num,7)<<endl;
	sort(Rs,Rs+245);
	cout<<"���̰���Q : "<<Rs[244]<<endl;
	cout<<"���̰����l : "<<Rs[0]<<endl;
	cout<<endl;
	cout<<"----------------------------"<<endl;
	cout<<endl;
	cout<<"��������Q(���Ҽ{����)"<<endl;
	cout<<"�`�@��Q : "<<trading(data,day_num,8)<<endl;
	sort(Rs,Rs+245);
	cout<<"���̰���Q : "<<Rs[244]<<endl;
	cout<<"���̧C��Q : "<<Rs[0]<<endl;
	cout<<endl;
	cout<<"----------------------------"<<endl;
	cout<<endl;
	cout<<"���������l(���Ҽ{����)"<<endl;
	cout<<"�`�@���l : "<<trading(data,day_num,9)<<endl;
	sort(Rs,Rs+245);
	cout<<"���̧C���l : "<<Rs[244]<<endl;
	cout<<"���̰����l : "<<Rs[0]<<endl;
	cout<<endl;
	cout<<"----------------------------"<<endl;
	cout<<endl;
	cout<<"��������Q(�Ҽ{����)"<<endl;
	cout<<"�`�@��Q : "<<trading(data,day_num,10)<<endl;
	sort(Rs,Rs+245);
	cout<<"���̰���Q : "<<Rs[244]<<endl;
	cout<<"���̤p��Q : "<<Rs[0]<<endl;
	return 0;
}
