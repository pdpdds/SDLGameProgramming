#pragma once
#include <vector>
using namespace std;

struct ParticleAnimationStructure
{
	SDL_Rect frame;
	int pivot;
	SDL_Rect collider;
	int duration;
};

class ParticleAnimation
{
public:
	vector<ParticleAnimationStructure> frames;
	bool loop;
	bool endReached;

private:
	int current_frame;
	int frames_in_frame;

public:
	ParticleAnimation() : frames(), current_frame(0), frames_in_frame(0), loop(true), endReached(false)
	{}

	~ParticleAnimation()
	{
		frames.clear();
	}

	SDL_Rect& GetCurrentFrame()
	{
		return frames[current_frame].frame;
	}

	int& GetCurrentPivot()
	{
		return frames[current_frame].pivot;
	}

	SDL_Rect& GetCurrentCollider()
	{
		return frames[current_frame].collider;
	}

	void NextFrame()
	{
		++frames_in_frame;
		if (frames[current_frame].duration == frames_in_frame)
		{
			frames_in_frame = 0;
			++current_frame;
		}
		if (current_frame == frames.size() && !loop){
			endReached = true;
			--current_frame;
		}
		else if (current_frame == frames.size())
		{
			frames_in_frame = 0;
			current_frame = 0;
		}
	}

	bool IsEnded()
	{
		return endReached;
	}

	void RestartFrames()
	{
		endReached = false;
		current_frame = 0;
		frames_in_frame = 0;
	}
};