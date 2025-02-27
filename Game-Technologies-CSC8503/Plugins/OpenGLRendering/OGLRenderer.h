/*
Part of Newcastle University's Game Engineering source code.

Use as you see fit!

Comments and queries to: richard-gordon.davison AT ncl.ac.uk
https://research.ncl.ac.uk/game/
*/
#pragma once
#include "../../Common/RendererBase.h"

#include "../../Common/Vector3.h"
#include "../../Common/Vector4.h"


#ifdef _WIN32
#include "windows.h"
#endif

#ifdef _DEBUG
#define OPENGL_DEBUGGING
#endif


#include <string>
#include <vector>

namespace NCL {
	class MeshGeometry;
	namespace Rendering {
		class ShaderBase;
		class TextureBase;

		class OGLMesh;
		class OGLShader;

		class SimpleFont;

		class OGLRenderer : public RendererBase
		{
		public:
			friend class OGLRenderer;
			OGLRenderer(Window& w);
			~OGLRenderer();

			void OnWindowResize(int w, int h)	override;
			bool HasInitialised()				const override {
				return initState;
			}

			void ForceValidDebugState(bool newState) {
				forceValidDebugState = newState;
			}

			virtual bool SetVerticalSync(VerticalSyncState s);

			void DrawString(const std::string& text, const Vector2&pos, const Vector4& colour = Vector4(0.75f, 0.75f, 0.75f,1));
			void DrawLine(const Vector3& start, const Vector3& end, const Vector4& colour);
			void DrawCircle(const Vector3& position, float radius, const Vector4& colour = Vector4(0.75f, 0.75f, 0.75f, 1));

			void SetAsActiveContext();
			virtual void SetupDebugMatrix(OGLShader*s) {

			}

		protected:			
			void BeginFrame()	override;
			void RenderFrame()	override;
			void EndFrame()		override;

			void DrawDebugData();
			void DrawDebugStrings();
			void DrawDebugLines();

			// 30.11.2019 - Ori Lazar
			
			void DrawDebugCircles();

			void BindShader(ShaderBase*s);
			void BindTextureToShader(const TextureBase*t, const std::string& uniform, int texUnit) const;
			void BindMesh(MeshGeometry*m);
			void DrawBoundMesh(int subLayer = 0, int numInstances = 1);
#ifdef _WIN32
			void InitWithWin32(Window& w);
			void DestroyWithWin32();
			HDC		deviceContext;		//...Device context?
			HGLRC	renderContext;		//Permanent Rendering Context		
#endif
		private:
			struct DebugString {
				Maths::Vector4 colour;
				Maths::Vector2	ndcPos;
				float			size;
				std::string		text;
			};

			struct DebugLine {
				Maths::Vector3 start;
				Maths::Vector3 end;
				Maths::Vector4 colour;
			};

			// 30.11.2019 - Ori Lazar
			struct DebugCircle
			{
				Maths::Vector4 colour;
				Maths::Vector3 position;
				float radius; 
			};

			OGLMesh* circleMesh;
			
			OGLMesh*	boundMesh;
			OGLShader*	boundShader;

			OGLShader*  debugShader;
			SimpleFont* font;
			std::vector<DebugString>	debugStrings;
			std::vector<DebugLine>		debugLines;
			std::vector<DebugCircle>	debugCircles;

			bool initState;
			bool forceValidDebugState;
		};
	}
}