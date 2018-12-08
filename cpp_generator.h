#include <iostream>
#include <memory>
#include <utility>
#include <functional>

namespace cpp_generator {
    class Coroutine;
    struct CoroutineContext;
    typedef void (*CoroEntry)(Coroutine*,void*);

    struct CoroutineTerminate {
    };

    class Coroutine
    {
        friend struct CoroutineManager;
    private:
        Coroutine();
        ~Coroutine();

    public:
        void resume();
        void terminate();

        void yield();

        bool alive();

    public:
        static Coroutine * alloc(CoroEntry entry, void *data);
        static void free(Coroutine *p);

    private:
        CoroutineContext * m_pCtx;
    };

    template<typename T> class Generator;
    template<typename T>
    class Yield {
        friend class Generator<T>;
    public:
        template<typename F>
        Yield(const F & lambda)
            : m_func(lambda)
        {
            this->m_coro = Coroutine::alloc(_entry, this);
            _next();
        }
        ~Yield() {
            if (this->m_coro) {
                if (this->m_coro->alive()) {
                    this->m_coro->terminate();
                }
                Coroutine::free(this->m_coro);
            }
        }
    private:
        bool alive() {
            return this->m_coro && this->m_coro->alive();
        }

        static void _entry(Coroutine *, void *pThis) {
            ((Yield<T>*)pThis)->m_func(*(Yield<T>*)pThis);
        }

        void _next() {
            this->m_coro->resume();
        }
    public:
        void operator()(T x) {
            m_tmp = std::move(x);
            m_coro->yield();
        }

    private:
        T m_tmp;
        Coroutine * m_coro;
        std::function<void(Yield<T>&)> m_func;
    };

    template<typename T>
    class Generator {
    public:
        template<typename F>
        Generator(const F & lambda)
        {
            m_yield = std::make_shared<Yield<T>>(lambda);
        }

        Generator & begin() {
            return *this;
        }
        Generator & end() {
            return *this;
        }
        bool operator != (const Generator & rhs) const {
            return m_yield->alive();
        }

        T operator *() {
            return std::move(m_yield->m_tmp);
        }
        void operator ++() {
            m_yield->_next();
        }

    private:
        std::shared_ptr<Yield<T>> m_yield;
    };
}
