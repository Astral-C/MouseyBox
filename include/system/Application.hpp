#ifndef __MB_APPLICATION_H__
#define __MB_APPLICATION_H__
#include <audio/Audio.hpp>
#include <graphics/Graphics.hpp>
#include <memory>

#ifdef __SWITCH__
#include <switch.h>
#endif

namespace mb {

	class Application {
	private:
		std::string mApplicationName {"MouseyBoxApp"};

		uint64_t mPrevTime { 0 };
		uint64_t mCurTime { 0 };
		uint64_t mTargetFps { 60 };
		float mDelta { 0.0 };

	protected:
		bool mQuit { false };
		std::unique_ptr<Graphics::Window> mWindow;
		std::unique_ptr<Graphics::Renderer> mRenderer;
		std::unique_ptr<Audio::Mixer> mAudio;

	public:

		static bool SDLReady();

		virtual void Update(float);
		virtual bool Initialize(bool commandline=false, uint32_t w = 1280, uint32_t h = 720);
		void Run();
		void Frame();

		Application();
		Application(std::string);
		virtual ~Application();
	};

}
#endif
