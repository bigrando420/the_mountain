#ifndef ANIMATION_H
#define ANIMATION_H

// makes use of a lot of Telescope definitions, not meant to be included into your own project
// should be trivial for you to update it with your own defs & style though if you want to make use of it.
// Let me know what you think of it though, any issues, ideas, suggestions are more than welcome if you've got experience.
// I'm not entirely happy with the implementation (which will always be the case for literally everything I write lol)
// but this seems to be working pretty well so far. Could probably use a simplification pass though.

#define TM_AnimateExp(value, target, rate) value += ((target) - value) * (1 - Pow(2.f, -(rate) * APP_DT()))

#define TM_AnimCompleted(anim) (F32Equals((anim).current, (anim).target, 0.000001f))

typedef enum TM_AnimType TM_AnimType;
enum TM_AnimType
{
	ANIM_linear,
	ANIM_exp_in,
	ANIM_exp_out,
	ANIM_exp_in_out,
	ANIM_cubic_in,
	ANIM_cubic_out,
	ANIM_cubic_in_out,
};

typedef struct TM_Anim TM_Anim;
struct TM_Anim
{
	// all animations move linearly towards the target (rate * delta_time)
	F32 current;
	F32 target;
	F32 initial;
	F32 rate;
	TM_AnimType ease_type;
	// https://easings.net/
};

function void TM_AnimSetTarget(TM_Anim* anim, const F32 target, const TM_AnimType type, const F32 rate)
{
	if (F32Equals(anim->target, target, 0.0001f))
		return; // already targeted

	anim->initial = anim->current;
	anim->target = target;
	anim->rate = rate;
	anim->ease_type = type;
}

function void TM_Animate(TM_Anim* anim, F32 delta_t)
{
	B8 sign = Sign(anim->target - anim->current);
	anim->current += (sign * anim->rate * delta_t);
	if (Sign(anim->target - anim->current) != sign)
	{
		anim->current = anim->target; // sign flipped, target reached
	}
}

function F32 F32Alpha(const F32 current, const F32 start, const F32 target)
{
	if (target == start)
		return 0;

	F32 d = target - start;
	F32 result = (current - start) / d;
	Assert(result >= 0 && result <= 1.f);
	return result;
}

function F32 TM_AlphaExpIn(const F32 alpha)
{
	return (alpha == 0 ? 0 : Pow(2.f, 10.f * alpha - 10.f));
}
function F32 TM_AlphaExpOut(const F32 alpha)
{
	return (alpha == 1 ? 1 : 1.f - Pow(2.f, -10.f * alpha));
}
function F32 TM_AlphaExpInOut(const F32 alpha)
{
	return (alpha == 0 ? 0 : alpha == 1.f ? 1.f : alpha < 0.5f ? Pow(2.f, 20.f * alpha - 10.f) / 2.f : (2.f - Pow(2.f, -20.f * alpha + 10.f)) / 2.f);
}

function F32 TM_AlphaCubicIn(const F32 alpha)
{
	return Pow(alpha, 3.f);
}
function F32 TM_AlphaCubicOut(const F32 alpha)
{
	return 1.f - Pow(1.f - alpha, 3);
}
function F32 TM_AlphaCubicInOut(const F32 alpha)
{
	return (alpha < 0.5f ? 4.f * alpha * alpha * alpha : 1.f - Pow(-2.f * alpha + 2.f, 3.f) / 2.f);
}

// fade: 6 * powf(result, 5) - 15 * powf(result, 4) + 10 * powf(result, 3);
// ease: 1.0f - (1.0f - result) * (1.0f - result);
// ^ what're these??

// todo - get remainder from https://easings.net/

// 0 -> 1 -> 0
// 0.5f == 1.0f (middle of the sin wave)
function F32 TM_AlphaSinPeak(const F32 alpha)
{
	F32 sin = alpha;
	sin = (sinf((alpha - .25f) * 2.f * PiF32) / 2.f) + 0.5f;
	return sin;
}

function F32 TM_AnimGetAlphaEased(const TM_Anim anim)
{
	F32 alpha = F32Alpha(anim.current, anim.initial, anim.target);

	switch (anim.ease_type)
	{
	case ANIM_exp_in:
		alpha = TM_AlphaExpIn(alpha);
		break;
	case ANIM_exp_out:
		alpha = TM_AlphaExpOut(alpha);
		break;
	case ANIM_exp_in_out:
		alpha = TM_AlphaExpInOut(alpha);
		break;
	case ANIM_cubic_in:
		alpha = TM_AlphaCubicIn(alpha);
		break;
	case ANIM_cubic_out:
		alpha = TM_AlphaCubicOut(alpha);
		break;
	case ANIM_cubic_in_out:
		alpha = TM_AlphaCubicInOut(alpha);
		break;
	}

	if (anim.target < anim.initial) // backwards target
		alpha = 1.f - alpha;

	return alpha;
}

#endif