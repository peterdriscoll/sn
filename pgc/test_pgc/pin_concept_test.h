#pragma once

#include <cstdio>
#include <stdexcept>

struct Txn {
    int pins = 0;
    void Pin() { std::puts("pin");   ++pins; }
    void Unpin() { std::puts("unpin"); --pins; }
};

struct Obj {
    Txn* txn{};
    explicit Obj(Txn* t) : txn(t) {}
    Txn* GetTransaction() const { return txn; }

    void f() { std::puts("Obj::f"); }
    void g() { std::puts("Obj::g"); }
    void throws() { std::puts("Obj::throws"); throw std::runtime_error("boom"); }
};

struct Ref1 {
    struct PinProxy {
        Obj* p{};
        explicit PinProxy(Obj* obj) : p(obj) {
            if (p && p->GetTransaction()) p->GetTransaction()->Pin();
        }
        ~PinProxy() {
            if (p && p->GetTransaction()) p->GetTransaction()->Unpin();
        }
        Obj* operator->() { return p; }
        Obj& operator*() { return *p; }
        Obj* get() const { return p; }   // helper for the “bad split” demo
    };

    Obj* obj{};
    explicit Ref1(Obj* o) : obj(o) {}
    // operator-> returns a proxy that pins for the full expression
    PinProxy operator->() { return PinProxy(obj); }
    // keep it pinned across multiple ops:
    PinProxy Pinned() { return PinProxy(obj); }
};

void run() 
{
    Txn t; Obj o(&t); Ref1 r(&o);

    std::puts("== single call ==");
    r->f();
    // expected:
    // pin
    // Obj::f
    // unpin

    std::puts("== exception still unpins ==");
    try { r->throws(); }
    catch (...) { std::puts("caught"); }
    // expected:
    // pin
    // Obj::throws
    // unpin
    // caught

    std::puts("== multiple calls need a longer pin scope ==");
    {
        auto pin = r.Pinned();  // pin held until block ends
        pin->f();
        pin->g();
    }
    // expected:
    // pin
    // Obj::f
    // Obj::g
    // unpin

    std::puts("== split across statements (NOT pinned on 2nd line) ==");
    auto* p = r.operator->().get(); // proxy destroyed here -> unpin now
    p->f();                         // runs without a pin
    // expected:
    // pin
    // unpin
    // Obj::f
}
