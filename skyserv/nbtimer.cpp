

#include "nbtimer.h"


static void
       print_siginfo(siginfo_t *si)
       {
           timer_t *tidp;
           int oret;

           tidp = (timer_t*)si->si_value.sival_ptr;

           printf("    sival_ptr = %p; ", si->si_value.sival_ptr);
           printf("    *sival_ptr = 0x%lx\n", (long) *tidp);

           oret = timer_getoverrun(*tidp);
           if (oret == -1) {
               perror("timer_getoverrun");
           }
           else
               printf("    overrun count = %d\n", oret);
       }

static void
       sig_timer_handler(int sig, siginfo_t *si, void *uc)
       {
           /* Note: calling printf() from a signal handler is not
              strictly correct, since printf() is not async-signal-safe;
              see signal(7) */

           printf("Caught signal %d\n", sig);
           print_siginfo(si);
           signal(sig, SIG_IGN);
       }

MyTimer::MyTimer()
{
    this->m_timer_id = MyTimer::INV_TIMER_ID;
    this->m_intval = 0;
    this->m_started = false;
    this->m_is_single_shot = false;
    // printf("====%d\n", this->m_timer_id == MyTimer::INV_TIMER_ID);
}

MyTimer::MyTimer(bool single)
{
    new(this)MyTimer();
    this->m_is_single_shot = true;
}

MyTimer::~MyTimer()
{
    if (this->m_timer_id != MyTimer::INV_TIMER_ID) {
        timer_delete(this->m_timer_id);
    }
}
// timer_t MyTimer::INV_TIMER_ID = NULL;

void MyTimer::start(int msec)
{
    this->m_intval = msec;

    if (this->m_timer_id == MyTimer::INV_TIMER_ID) {
        this->init_timer_signal();
    }

    if (!this->m_started) {
        this->start();
    }
}

void MyTimer::start()
{
    int iret;

    if (this->m_timer_id == MyTimer::INV_TIMER_ID) {
        this->init_timer_signal();
    }

    if (!this->m_started) {
        struct itimerspec itspec, oitspec;
        memset(&itspec, 0, sizeof(itspec));
        itspec.it_interval.tv_sec = this->m_intval / 1000;
        itspec.it_interval.tv_nsec = (this->m_intval % 1000) * 1000000;
        // itspec.it_interval.tv_sec = 2;
        // itspec.it_interval.tv_nsec = 0;

        itspec.it_value.tv_sec = itspec.it_interval.tv_sec;
        itspec.it_value.tv_nsec = itspec.it_interval.tv_nsec;

        printf("sec: %ld, nsec: %ld\n", itspec.it_interval.tv_sec, itspec.it_interval.tv_nsec);

        memset(&oitspec, 0, sizeof(oitspec));
    
        iret = timer_settime(this->m_timer_id, 0, &itspec, NULL);
        if (iret == -1) {
            perror("set error");
            // assert(iret != -1);
        }

        // this->m_started = true;
        this->m_started.exchange(true);
    }
}

void MyTimer::stop()
{
    int iret = 0;
    if (this->m_timer_id != MyTimer::INV_TIMER_ID) {
        struct itimerspec itspec, oitspec;
        memset(&itspec, 0, sizeof(struct itimerspec));
        memset(&oitspec, 0, sizeof(struct itimerspec));
        
        iret = timer_settime(this->m_timer_id, 0, &itspec, &oitspec);
        if (iret == -1) {
            perror("set timer stop error");
        }
    }
}

void MyTimer::setInterval(int msec)
{
    // this->m_intval = msec;
    this->m_intval.exchange(msec);
}

void MyTimer::init_timer_signal()
{
    int iret = 0;
    sigset_t mask;
    struct sigaction sa;

    sa.sa_flags = SA_SIGINFO;
    // sa.sa_sigaction = sig_timer_handler;
    // sa.sa_sigaction = MyTimer::sig_timeout_handler;
    sa.sa_sigaction = MyTimer::sig_timeout_handler;
    sigemptyset(&sa.sa_mask);
    iret = sigaction(SIGRTMIN, &sa, NULL);
    assert(iret != -1);

    sigemptyset(&mask);
    sigaddset(&mask, SIGRTMIN);
    // iret = sigprocmask(SIG_SETMASK, &mask, NULL);

    this->m_sev.sigev_notify = SIGEV_SIGNAL;
    this->m_sev.sigev_signo = SIGRTMIN;
    // this->m_sev.sigev_value.sival_ptr = &this->m_timer_id;
    this->m_sev.sigev_value.sival_ptr = this;
    iret = timer_create(CLOCK_REALTIME, &this->m_sev, &this->m_timer_id);
    printf("timer:%p\n", this->m_timer_id);
    assert(iret != -1);

}

// #include "boost/lambda/lambda.hpp"
// #include "boost/lambda/bind.hpp"
// #include "boost/lambda/if.hpp"

struct abcde {
    void operator()(int a) {
        printf("ahaha: %d\n", a);
    }
};

boost::signals2::connection MyTimer::connect(boost::signals2::signal<void ()>::slot_type slot)
{
    boost::signals2::connection conn;

    conn = this->sig_timeout.connect(slot);

    return conn;

    boost::function<void(void)> onClick;

    // ok
    onClick = slot;

    // ok
    onClick = {
    };
    onClick();

    // ok
    onClick = slot;

    boost::function<void(int)> onChange;

    // ok
    onChange = abcde();

    // onChange = (std::cout<< boost::lambda::_1 <<" "<<boost::lambda::_1);
    // (std::cout<< boost::lambda::_1 << boost::lambda::_3 << boost::lambda::_2)("adf","fd","dfd");

    [](int x) -> int {
        return x + 6;
    };

    int x = 5;
    char *y = NULL;
    std::string z;

    [=](int x) -> int {
        if (y == NULL) {
        }
        return x + 6;
    };

    [&](int x) -> int {
        if (y == NULL) {
        }
        return x + 6;
    };

    [&y,&z](int x) -> int {
        if (y == NULL) {
        }
        return x + 6;
    };

    auto my_lambda_func = [&](int x) { /*...*/ };
    auto my_onheap_lambda_func = new auto([=](int x) { /*...*/ });
    my_lambda_func(5);
    (*my_onheap_lambda_func)(6);

    using namespace std;
    vector<int> v = {50, -10, 20, -30};

    // gcc version problem, go to gcc 4.6.0 now
    std::sort(v.begin(), v.end(), [](int a, int b) { return abs(a)<abs(b); });

    struct BasicStruct {
        int x;
        double y;
    };
 
    struct AltStruct {
        AltStruct(int x, double y) : x_{x}, y_{y} {}
 
    private:
        int x_;
        double y_;
    };
 
    BasicStruct var1{5, 3.2};
    AltStruct var2{2, 4.3};

}

// static 
void MyTimer::singleShot(int msec, boost::signals2::signal<void ()>::slot_type slot)
{
    MyTimer *t = new MyTimer(true);
    t->setInterval(msec);
    t->connect(slot);
    // t->connect(NBSIGNAL(t,sig_timeout2),slot);
    t->start();
}

// static 
void MyTimer::sig_timeout_handler(int sig, siginfo_t *si, void *uc)
{
    /* Note: calling printf() from a signal handler is not
       strictly correct, since printf() is not async-signal-safe;
       see signal(7) */

    printf("Caught signal %d\n", sig);
    [] (siginfo_t*si) {
        // MyTimer::print_siginfo(si);
        MyTimer::print_siginfo(si);
    } (si);


    MyTimer::sig_timeout_revoke_back1(si);
    // signal(sig, SIG_IGN);
}

// static 
void MyTimer::print_siginfo(siginfo_t *si)
{
    timer_t *tidp;
    int oret;

    // tidp = (timer_t*)si->si_value.sival_ptr;

    // printf("    sival_ptr = %p; ", si->si_value.sival_ptr);
    // printf("    *sival_ptr = 0x%lx\n", (long) *tidp);

    // oret = timer_getoverrun(*tidp);
    // if (oret == -1) {
    //     perror("timer_getoverrun");
    // }
    // else
    //     printf("    overrun count = %d\n", oret);

    
}

// static 
void MyTimer::sig_timeout_revoke_back1(siginfo_t *si)
{
    MyTimer *t = NULL;

    t = (MyTimer*)si->si_value.sival_ptr;
    t->sig_timeout_revoke_back2();
}

void MyTimer::sig_timeout_revoke_back2()
{
    printf("sig timeout arrived:\n");
    this->sig_timeout();
    if (this->m_is_single_shot) {
        this->stop();
        delete this;
    }
}
