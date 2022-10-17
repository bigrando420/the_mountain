#ifndef TM_COROUTINE
#define TM_COROUTINE

// NOTE - This just serves as an example of a very simple "just get it working" game-level code wrapping of Minicoro

// I'd love to get this comment block rendered out to a .md file that I can edit within obsidian. Write some kinda utility program that takes the .md file and keeps it in sync with this comment
// I just want to be able to write these notes within obsidian to make use of linking, hotkeys and whatnot.
/* TODO
   
- [ ] more robust wrapping of minicoro

*/

// https://github.com/edubart/minicoro
#define MINICORO_IMPL
#include "ext/minicoro.h"

// just helpers for now, not wrapped properly

/*
typedef struct TM_Coroutine TM_Coroutine;
struct TM_Coroutine
{
    mco_coro* co;
};

typedef void(TM_CoroutineFunction)(TM_Coroutine* co);
*/

// do I bother wrapping minicoro?
// probs so I can keep the api pretty solid
// who tf cares tho, shouldn't I just get it up and running first?
// yeeeeeeeeeeeee fuck it

// check https://github.com/RandyGaul/cute_framework/blob/master/src/cute_coroutine.cpp for a solid example of a wrap


#define TM_CoroWaitForSeconds(duration) { U64 start_time = 0;\
if (start_time == 0) { start_time = OS_TimeMicroseconds(); }\
while (OS_TimeMicroseconds() < start_time + TM_SecondToMicroF32(duration)) { TM_CoroutineYield(co); } }

#define TM_CoroWaitForAnim(_anim) while (!TM_AnimCompleted(_anim)) { TM_CoroutineYield(co); }

function mco_coro* TM_CoroutineCreate(void (*func)(mco_coro* co_))
{
    // todo - replace alloaction with memory arena pattern for optimal chaddery

    mco_desc desc = mco_desc_init(func, 0);
    desc.user_data = NULL;

    mco_coro* co;
    mco_result res = mco_create(&co, &desc);
    Assert(res == MCO_SUCCESS);
    Assert(mco_status(co) == MCO_SUSPENDED);

    return co;
}

function void TM_CoroutineResume(mco_coro* co)
{
    Assert(mco_status(co) == MCO_SUSPENDED); // wasn't suspended to begin with??
    // what should we do in this situation?

    mco_result res = mco_resume(co);
    Assert(res == MCO_SUCCESS);

    Assert(mco_status(co) == MCO_SUSPENDED || mco_status(co) == MCO_DEAD);
    // if it's dead, should I just auto delete it here?
    // probs just wait til I do a memory arena rework on this so I don't have to worry about this issue lol
}

function void TM_CoroutineYield(mco_coro* co)
{
    mco_yield(co);
}

function void TM_CoroutineDestroy(mco_coro* co)
{
    Assert(mco_status(co) == MCO_DEAD); // not fully complete yet?

    mco_result res = mco_destroy(co);
    Assert(res == MCO_SUCCESS);
}

/*

function void ExampleRoutine(mco_coro* co)
{
    Log("coroutine 1\n");
    TM_CoroutineYield(co);

    Log("coroutine 2\n");
    TM_CoroutineYield(co);

    Log("coroutine 3\n");
}

*/

#endif