#include<iostream>
#include<type_traits>
#include<tuple>
#include<functional>
using namespace std;
////////////////////////////
/*
	�����߽�
	1.����ָ��
	2.������ //���js function curring 
*/
/////////////////////////// 
void add(int a,int b){
	cout<<a+b<<endl;
} 
class Test{
	public:
		int add(int a,int b)const{
			sum++;
			cout<<a+b<<"  sum: "<<sum<<endl;
			return a+b;
		}
		int minus(int a,int b)const{
			sum++;
			cout<<a-b<<"  sum: "<<sum<<endl;
			return a-b;
		}
		static int mul(int a,int b);
//		{
//			cout<<a*b<<endl;
//			return a*b;
//		}
	private:
		mutable int sum=0;
}; 
int Test::mul(int a,int b){
	cout<<a*b<<endl;
	return a*b;
}
typedef int(Test::*fn_add)(int,int)const;

class Fn{
	public:
		int sum=0;
		void operator()(int a){
			sum+=a;
		}
	private:
}; 

//struct ignore{//������bindʵ�� 
//	//nothing to do
//};
//template<class F,class ...Args>
//auto m_bind(F fn,Args ...args)->decltype(){
//	return fn
//}

template<class Tp,class Fn,size_t N>
struct __do_tp_foreach{
	static void run(Tp& t,Fn fn){
		__do_tp_foreach<Tp,Fn,N-1>::run(t,fn);
		fn(get<N>(t));
	}
};
template<class Tp,class Fn>
struct __do_tp_foreach<Tp,Fn,0>{
	static void run(Tp& t,Fn fn){
		fn(get<0>(t));
	}
};
template<class Fn,class ...Args>
void tp_foreach(const tuple<Args...>& t,const Fn fn){
	typedef const tuple<Args...>& tp_type;
	__do_tp_foreach<tp_type,Fn,tuple_size<tp_type>::value-1>::run(t,fn);
}
template<class T>
void doub(T& a){
	a*=2;
} 
template<class T>
void tp_print(T& a){
	cout<<a<<',';
}

int main(){
	cout<<"-------------"<<endl;//��ͨ����ָ�� 
	
	void (*pfn0)(int,int)=add;//��ͨ�������� & 
	pfn0(1,2);//Ҳ���� * 
	
	cout<<"-------------"<<endl;//��Ա����ָ�� 
	
	int (Test::*pfn1)(int,int)const=&Test::add;//����� &
	fn_add pfn2=&Test::minus;
	const Test t;
	//Test::add(1,2);//��Ա����ʹ�ñ���ʵ���� 
	//pfn1(1,2);//�����ж���ʵ�� 
	(t.*pfn1)(1,2);
	(t.*pfn2)(1,2);
	
	int (*a)(int,int)=Test::mul;//static������Ϊ��������֮�⣬����ͨ����һ�� 
	Test::mul(1,2);//static������ֱ���� 
	
	Test t2;
	function<int(const Test&,int,int)> pfn3=&Test::add;//��ʵ֤��function�ǳ����㣬ע��Ҫ����this 
	pfn3(t,1,2);
	pfn3(t2,1,2);
	//��û��this
	Fn fn_t;//�ຯ����functor 
	function<void(int)> pfn4=fn_t;//�ɼ�function�ǿ������ݣ���ֵ���ݣ� 
	pfn4(10);
	pfn4(10);
	cout<<fn_t.sum<<endl;
	function<void(int)> pfn5=ref(reference_wrapper<Fn>(fn_t));//��׼�÷�����ʵ�ϲ���ref_warp,������ 
	pfn5(101);
	cout<<fn_t.sum<<endl; 
	function<void(int)> pfn6=bind(fn_t,placeholders::_1);
	pfn6(3);
	cout<<fn_t.sum<<endl; 
	
	cout<<"-------------"<<endl;//function ,bind ,lamba
	//https://www.cnblogs.com/yyxt/p/4253088.html
	//https://blog.csdn.net/u012297622/article/details/51303238
	//https://www.cnblogs.com/yyxt/p/3987717.html 
	
	function<void(int,int)> fna=add;
/*//ʾ�����룺bind 
#include <iostream>
using namespace std;
class A
{
public:
    void fun_3(int k,int m)
    {
        cout<<k<<" "<<m<<endl;
    }
};

void fun(int x,int y,int z)
{
    cout<<x<<"  "<<y<<"  "<<z<<endl;
}

void fun_2(int &a,int &b)
{
    a++;
    b++;
    cout<<a<<"  "<<b<<endl;
}

int main(int argc, const char * argv[])
{
    //f1������Ϊ function<void(int, int, int)>
    auto f1 = std::bind(fun,1,2,3); //��ʾ�󶨺��� fun �ĵ�һ��������������ֵΪ�� 1 2 3
    f1(); //print:1  2  3

    auto f2 = std::bind(fun, placeholders::_1,placeholders::_2,3);
    //��ʾ�󶨺��� fun �ĵ���������Ϊ 3����fun �ĵ�һ�����������ֱ��ɵ��� f2 �ĵ�һ����������ָ��
    f2(1,2);//print:1  2  3

    auto f3 = std::bind(fun,placeholders::_2,placeholders::_1,3);
    //��ʾ�󶨺��� fun �ĵ���������Ϊ 3����fun �ĵ�һ�����������ֱ��ɵ��� f3 �ĵڶ���һ������ָ��
    //ע�⣺ f2  ��  f3 ������
    f3(1,2);//print:2  1  3


    int n = 2;
    int m = 3;

    auto f4 = std::bind(fun_2, n,placeholders::_1); //��ʾ��fun_2�ĵ�һ������Ϊn, fun_2�ĵڶ��������ɵ���f4�ĵ�һ��������_1��ָ����
    f4(m); //print:3  4

    cout<<m<<endl;//print:4  ˵����bind���ڲ����Ȱ󶨵Ĳ�����ͨ��std::placeholders���ݵĲ�����ͨ�����ô��ݵ�,��m
    cout<<n<<endl;//print:2  ˵����bind����Ԥ�Ȱ󶨵ĺ���������ͨ��ֵ���ݵģ���n


    A a;
    //f5������Ϊ function<void(int, int)>
    auto f5 = std::bind(&A::fun_3, a,placeholders::_1,placeholders::_2); //ʹ��auto�ؼ���
    f5(10,20);//����a.fun_3(10,20),print:10 20

    std::function<void(int,int)> fc = std::bind(&A::fun_3, a,std::placeholders::_1,std::placeholders::_2);
    fc(10,20);//����a.fun_3(10,20) print:10 20 

����return 0; 
}
*/

	cout<<"-------------"<<endl;//tp_foreach with bind 
	auto tp=make_tuple(1,2,3,"qwert",12.5);//ע�⣬��tie��forward_as_tuple���ܿ� 
	//[](auto& a){return ++a;}(qwert);//������ģ���������Ϊauto 
	tp_foreach(tp,doub);
	tp_foreach(tp,tp_print);
	
	return 0;
}
