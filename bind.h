#pragma once

#include <type_traits>
#include <tuple>

/*
	��̬�ѣ����������� 
	 https://www.cnblogs.com/qicosmos/p/3723388.html ���У���ϧ���ĳ�������е����⣩ 
*/ 

namespace simple {

// placeholder

template <int N>
struct placeholder {};

static placeholder<1> _1; static placeholder<6>  _6;  static placeholder<11> _11; static placeholder<16> _16;
static placeholder<2> _2; static placeholder<7>  _7;  static placeholder<12> _12; static placeholder<17> _17;
static placeholder<3> _3; static placeholder<8>  _8;  static placeholder<13> _13; static placeholder<18> _18;
static placeholder<4> _4; static placeholder<9>  _9;  static placeholder<14> _14; static placeholder<19> _19;
static placeholder<5> _5; static placeholder<10> _10; static placeholder<15> _15; static placeholder<20> _20;

// sequence & generater
// ������������ ? ?

template <int... N>
struct seq { typedef seq<N..., sizeof...(N)> next_type; };
//�������У�
// seq<0>::next_type == seq<0,1>
// seq<0>::next_type::next_type == seq<0,1,2>

template <typename... P>
struct gen;

template <>
struct gen<>
{
    typedef seq<> seq_type;
};

template <typename P1, typename... P>
struct gen<P1, P...>
{
    typedef typename gen<P...>::seq_type::next_type seq_type;// ? ��Ӧ���ǿհ�? 
};

// select	// ?

template <typename T, class TupleT>
inline auto select(T&& val, TupleT& /*tp*/) -> T&&
{
    return std::forward<T>(val);
}

template <int N, class TupleT>
inline auto select(placeholder<N>, TupleT& tp) -> decltype(std::get<N - 1>(tp))
{
    return std::get<N - 1>(tp);
}

// result type traits

template <typename F>
struct result_traits : result_traits<decltype(&F::operator())> {};// ȡ�º����������� 

template <typename T>
struct result_traits<T*> : result_traits<T> {};// ȡ��ָ�����ͣ��ټ����ж� 

/* check function */

template <typename R, typename... P>//ȡ��ͨ��������Ա������������ 
struct result_traits<R(*)(P...)> { typedef R type; };

/* check member function */

#define RESULT_TRAITS__(...) \
    template <typename R, typename C, typename... P> \
    struct result_traits<R(C::*)(P...) __VA_ARGS__> { typedef R type; };

RESULT_TRAITS__()
RESULT_TRAITS__(const)
RESULT_TRAITS__(volatile)
RESULT_TRAITS__(const volatile)

#undef RESULT_TRAITS__

// The invoker for call a callable
// ����һ���ɵ��õĳ���  ���䣿 

template <typename T>//�Ƿ�Ϊָ�� 
struct is_pointer_noref
    : std::is_pointer<typename std::remove_reference<T>::type>
{};

template <typename T>//�Ƿ�Ϊ��Ա���� 
struct is_memfunc_noref
    : std::is_member_function_pointer<typename std::remove_reference<T>::type>
{};
/////////////�����󶨵�ִ�г��� 
template <typename R, typename F, typename... P>//��ͨ����ָ��
inline typename std::enable_if<is_pointer_noref<F>::value,
R>::type invoke(F&& f, P&&... par)
{
    return (*std::forward<F>(f))(std::forward<P>(par)...);
}

template <typename R, typename F, typename P1, typename... P>//����ָ��
inline typename std::enable_if<is_memfunc_noref<F>::value && is_pointer_noref<P1>::value,
R>::type invoke(F&& f, P1&& this_ptr, P&&... par)
{
    return (std::forward<P1>(this_ptr)->*std::forward<F>(f))(std::forward<P>(par)...);
}

template <typename R, typename F, typename P1, typename... P>//�����Ա���� 
inline typename std::enable_if<is_memfunc_noref<F>::value && !is_pointer_noref<P1>::value,
R>::type invoke(F&& f, P1&& this_obj, P&&... par)
{
    return (std::forward<P1>(this_obj).*std::forward<F>(f))(std::forward<P>(par)...);
}

template <typename R, typename F, typename... P>//��ͨ���� 
inline typename std::enable_if<!is_pointer_noref<F>::value && !is_memfunc_noref<F>::value,
R>::type invoke(F&& f, P&&... par)
{
    return std::forward<F>(f)(std::forward<P>(par)...);
}

// Simple functor for bind function's return type

template<typename FuncT, typename... ArgsT>
class fr
{
	//private
    typedef std::tuple<typename std::decay<ArgsT>::type...> args_type;//�������� 
    typedef typename std::decay<FuncT>::type                callable_type;//������ 
    typedef typename result_traits<callable_type>::type     result_type;//����ֵ 

    callable_type call_;//������ 
    args_type     args_;//�������� 

    template <class TupleT, int... N>
    result_type do_call(TupleT& tp, seq<N...>)//���� 
    {
        return invoke<result_type>(call_, select(std::get<N>(args_), tp)...);
    }

public:
    template<typename F, typename... P>//��ʼ�� 
    fr(F&& f, P&&... par)
        : call_(std::forward<F>(f))
        , args_(std::forward<P>(par)...)
    {}

    template <typename... P>//���� 
    result_type operator()(P&&... par)
    {
        /*
            <MSVC 2013> type_traits(1509): fatal error C1001
            With: std::forward<P>(par)...
        */
        std::tuple<typename std::decay<P>::type...> pars(static_cast<P&&>(par)...);// ? 
        return do_call(pars, typename gen<ArgsT...>::seq_type());
    }
};

// Bind function arguments

template <typename F, typename... P>
inline fr<F, P...> bind(F&& f, P&&... par)//һ��С��װ 
{
    return fr<F, P...>(std::forward<F>(f), std::forward<P>(par)...);
}

} // namespace simple
