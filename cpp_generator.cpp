#include <vector>
#include "cpp_generator.h"
#include "libcoro/coro.h"

namespace cpp_generator {

///////////////////////////////////////////////////////////////////////////////
// Coroutine

    void coro_body(void * p);

    struct CoroutineContext {
        coro_context ctx;
        coro_stack stack;
        Coroutine * coro = nullptr;
        CoroEntry entry = nullptr;
        void * data = nullptr;
        bool termself = false;

        CoroutineContext(bool isMainCtx) {
            coro_stack_alloc(&stack, 0);
            coro_create(&ctx, coro_body, this, stack.sptr, stack.ssze);
        }
        ~CoroutineContext() {
            coro_destroy(&ctx);
            coro_stack_free(&stack);
        }
    };

    struct CoroutineManager {
        coro_context main;
        coro_context * cur;
        coro_context * prev;

        std::vector<Coroutine *> pool;

        CoroutineManager() {
            coro_create(&main, nullptr, nullptr, nullptr, 0);
            cur = &main;
            prev = nullptr;
        }
        ~CoroutineManager() {
            coro_destroy(&main);
        }

        Coroutine * alloc(CoroEntry entry, void * data) {
            Coroutine * p;
            if (pool.empty()) {
                p = new Coroutine();
            } else {
                p = pool.back();
                pool.pop_back();
            }

            p->m_pCtx->entry = entry;
            p->m_pCtx->data = data;

            return p;
        }
        void free(Coroutine * p) {
            if (p->alive())
                p->terminate();
            pool.push_back(p);
        }

        void enter(coro_context * ctx) {
            coro_context * pprev = prev;
            prev = cur;
            cur = ctx;
            coro_transfer(prev, cur);
            cur = prev;
            prev = pprev;
        }

        void leave() {
            coro_transfer(cur, prev);
        }
    };

    static thread_local CoroutineManager ts_coro_mgr;

    [[noreturn]] void coro_body(void * p) {
        CoroutineContext * pCtx = (CoroutineContext*)p;
        while (true) {
            if (pCtx->entry) {
                try {
                    pCtx->entry(pCtx->coro, pCtx->data);
                } catch (CoroutineTerminate &e) {
                    // do nothing
                }
                pCtx->entry = nullptr;
                pCtx->data = nullptr;
            }
            ts_coro_mgr.leave();
            pCtx->termself = false;
        }
    }

    Coroutine::Coroutine() {
        m_pCtx = new CoroutineContext(false);
        m_pCtx->coro = this;
    }

    Coroutine::~Coroutine() {
        if (alive())
            terminate();
        delete m_pCtx;
    }

    void Coroutine::resume() {
        ts_coro_mgr.enter(&m_pCtx->ctx);
    }

    void Coroutine::terminate() {
        m_pCtx->termself = true;
        resume();
    }

    void Coroutine::yield() {
        ts_coro_mgr.leave();
        if (m_pCtx->termself) {
            m_pCtx->termself = false;
            throw CoroutineTerminate();
        }
    }

    bool Coroutine::alive() {
        return !!m_pCtx->entry;
    }

    Coroutine * Coroutine::alloc(CoroEntry entry, void * data) {
        return ts_coro_mgr.alloc(entry, data);
    }

    void Coroutine::free(Coroutine * p) {
        return ts_coro_mgr.free(p);
    }
}
