#ifndef NBTIMER_H
#define NBTIMER_H

#include <signal.h>
#include <sys/time.h>
#include <atomic>
#include "boost/signals2.hpp"
#include "boost/type_traits.hpp"
#include <typeinfo>
#include <cxxabi.h>

#define NBSLOT(obj,mf) boost::bind(&mf,obj)
#define NBSLOT1(mf,obj) boost::bind(&mf,obj,_1)
#define NBSLOT2(mf,obj) boost::bind(&mf,obj,_1,_2)
#define NBSLOT3(mf,obj) boost::bind(&mf,obj,_1,_2,_3)
#define NBSLOT4(mf,obj) boost::bind(&mf,obj,_1,_2,_3,_4)
#define NBSLOT5(mf,obj) boost::bind(&mf,obj,_1,_2,_3,_4,_5)
#define NBSLOT6(mf,obj) boost::bind(&mf,obj,_1,_2,_3,_4,_5,_6)
#define NBSLOT7(mf,obj) boost::bind(&mf,obj,_1,_2,_3,_4,_5,_6,_7)
#define NBSLOT8(mf,obj) boost::bind(&mf,obj,_1,_2,_3,_4,_5,_6,_7,_8)
#define NBSLOT9(mf,obj) boost::bind(&mf,obj,_1,_2,_3,_4,_5,_6,_7,_8,_9)

// connect(NBSIGNAL(timer,sig_timeout2),NBSLOT(handle,mf));
#define NBSIGNAL(obj,mf) obj->mf


namespace XObject {
    template<typename Z>
    inline int mfn_arity(Z mf) {
        char   *realname, *p1;
        int     status;
        int arity = 0;
        const std::type_info &tia = typeid(mf);

        realname = abi::__cxa_demangle(tia.name(), 0, 0, &status);
        while (p1++) {
            if (*p1 == ',') arity++;
        }
        
        free(realname);

        return arity;
    }

    // #define NBSLOTn(obj,mf) mfn_arity(&mf) == 0 ? (boost::bind(&mf, obj)) : (boost::bind(&mf, obj, _1, _2, _3))

    template<typename Y, typename Z>
    auto NBSLOTx(Y obj, Z mf) -> decltype(boost::bind(mf, obj, _1, _2, _3)) {
        Y __a;
        auto f0 = boost::bind(mf, obj);
        // auto f1 = boost::bind(mf, obj, _1);
        auto f3 = boost::bind(mf, obj, _1, _2, _3);

        // 
        char   *realname, *p1;
        int     status;
        int arg_cnt = 0;

        decltype(mf) tf;
        boost::remove_pointer<decltype(mf)> __b;
        const std::type_info &ti2 = typeid(mf);
        // int __an = boost::function_traits<void (int, int, int)>::arity;

        realname = abi::__cxa_demangle(ti2.name(), 0, 0, &status);
        while (p1++) {
            if (*p1 == ',') arg_cnt++;
        }
        
        free(realname);

        return boost::bind(mf, obj, _1, _2, _3);
    }

    template<typename Y, typename Z>
    boost::signals2::connection connect(Y &st, Z slot)
    {
        boost::signals2::connection conn;

        conn = st.connect(slot);
        // switch (st) {
        // case MyTimer::sig_timeout2:

        //     break;
        // };
        
        return conn;
    }

    template<typename Y, typename Z>
    boost::signals2::connection connect2(Y &st, Z slot)
    {
        boost::signals2::connection conn;
        // decltype(st)::slot_type & ab;
        typedef typename Y::slot_type xslot_type;
        // switch (mfn_arity(mf)) {
        // case 0:
        //     // 
        //     // xslot_type fn = boost::bind(mf, obj);
        //     // st.connect(fn);
        //     break;
        // }
        conn = st.connect(slot);
        // conn = st.connect(slot);
        // switch (st) {
        // case MyTimer::sig_timeout2:

        //     break;
        // };
        
        return conn;
    }

};

class MyTimer
{
 public:
    MyTimer();
    virtual ~MyTimer();

    void start(int msec);
    void start();
    void stop();
    int interval() {return this->m_intval.load();}
    void setInterval(int msec);
    bool isSingleShot() { return this->m_is_single_shot.load(); }
    bool isActive() {return this->m_started.load(); }
    
    boost::signals2::connection connect(boost::signals2::signal<void ()>::slot_type slot);
    template<typename Y, typename Z>
    boost::signals2::connection connect(Y &st, Z slot)
    {
        boost::signals2::connection conn;

        conn = st.connect(slot);
        // switch (st) {
        // case MyTimer::sig_timeout2:

        //     break;
        // };
        
        return conn;
    }

    static void singleShot(int msec, boost::signals2::signal<void ()>::slot_type slot);

 public:
    static void sig_timeout_handler(int sig, siginfo_t *si, void *uc);
    static void print_siginfo(siginfo_t *si);
    
    static void sig_timeout_revoke_back1(siginfo_t *si);
    void sig_timeout_revoke_back2();

 public: // signals
    enum {
        // sig_timeout2,
    };
    boost::signals2::signal<void ()> sig_timeout;
    boost::signals2::signal<void ()> sig_timeout2;

 protected:
    MyTimer(bool single);
    void init_timer_signal();

 private:
    timer_t m_timer_id;
    std::atomic<int> m_intval;
    struct sigevent m_sev;
    std::atomic<bool> m_started;
    std::atomic<bool> m_is_single_shot;
    static constexpr timer_t  INV_TIMER_ID = NULL;
};

#endif
