// FFMPGE.cpp : �������̨Ӧ�ó������ڵ㡣
//
#pragma comment(lib, "libavcodec.a")
#pragma comment(lib, "libavformat.a")
#pragma comment(lib, "libavutil.a")
#pragma comment(lib, "libswscale.a")
#pragma comment(lib, "libswresample.a")

#include "stdafx.h"
#include "libav\avplay.h"
#include "audio\audio_out.h"
#include "video\video_out.h"
#include "libav\globals.h"

// ��Ⱦģʽ.
#define RENDER_DDRAW	0
#define RENDER_D3D		1
#define RENDER_OGL		2
#define RENDER_SOFT		3
#define RENDER_Y4M		4
HANDLE fd;

int file_init_source(struct source_context *ctx)
{
	// �õ��ļ���Ϣ������, ����ȡ���ļ�����Ϣ.
	file_source_info &file = ctx->info.file;
	fd = ::CreateFileA(file.file_name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, OPEN_EXISTING, 0, 0);//���ļ�

	// ���ļ�.
	return 0;
}

int64_t file_read_data(struct source_context *ctx, char* buff, size_t buf_size)
{
	DWORD size;
	ReadFile(fd, buff, buf_size, &size, 0);

	return size;
}

int64_t file_read_seek(struct source_context *ctx, int64_t offset, int whence)
{
	LARGE_INTEGER li = { 0 }; //�ǵó�ʼ��
	li.QuadPart = offset/*�ƶ���λ��*/;
	
	switch (whence)
	{
	case SEEK_SET:	// �ļ���ʼλ�ü���.
	{
						return SetFilePointer(fd, li.LowPart, &li.HighPart, FILE_BEGIN);
					
	}
		break;
	case SEEK_CUR:	// �ļ�ָ�뵱ǰλ�ÿ�ʼ����.
	{

						return SetFilePointer(fd, li.LowPart, &li.HighPart, FILE_CURRENT);
		
	}
		break;
	case SEEK_END:	// �ļ�β��ʼ����.
	{
						return  SetFilePointer(fd, li.LowPart, &li.HighPart, FILE_END);
						 
	}
		break;
	default:
		break;
	}
	return -1;
}

void file_close(struct source_context *ctx)
{

}

void file_destory(struct source_context *ctx)
{

}


void init_audio(ao_context *ao)
{

	ao->init_audio = dsound_init_audio;
	ao->play_audio = dsound_play_audio;
	ao->audio_control = dsound_audio_control;
	ao->mute_set = dsound_mute_set;
	ao->destory_audio = dsound_destory_audio;
}

void init_video(vo_context *vo, int render_type/* = RENDER_SOFT*/)
{
	int ret = 0;
	int check = 0;

	do
	{
		if (render_type == RENDER_D3D || check == -1)
		{
			ret = d3d_init_video(vo, 10, 10, PIX_FMT_YUV420P);
			d3d_destory_render(vo);
			if (ret == 0)
			{
				vo->init_video = d3d_init_video;
				vo->re_size = d3d_re_size;
				vo->aspect_ratio = d3d_aspect_ratio;
				vo->use_overlay = d3d_use_overlay;
				vo->destory_video = d3d_destory_render;
				vo->render_one_frame = d3d_render_one_frame;
				break;
			}
		}
		if (render_type == RENDER_DDRAW || check == -1)
		{
			ret = ddraw_init_video(vo, 10, 10, PIX_FMT_YUV420P);
			ddraw_destory_render(vo);
			if (ret == 0)
			{
				vo->init_video = ddraw_init_video;
				vo->re_size = ddraw_re_size;
				vo->aspect_ratio = ddraw_aspect_ratio;
				vo->use_overlay = ddraw_use_overlay;
				vo->destory_video = ddraw_destory_render;
				vo->render_one_frame = ddraw_render_one_frame;
				break;
			}
		}
		if (render_type == RENDER_OGL || check == -1)
		{
			ret = ogl_init_video(vo, 10, 10, PIX_FMT_YUV420P);
			ogl_destory_render(vo);
			if (ret == 0)
			{
				vo->init_video = ogl_init_video;
				vo->re_size = ogl_re_size;
				vo->aspect_ratio = ogl_aspect_ratio;
				vo->use_overlay = ogl_use_overlay;
				vo->destory_video = ogl_destory_render;
				vo->render_one_frame = ogl_render_one_frame;
				break;
			}
		}
		if (render_type == RENDER_SOFT || check == -1)
		{
			ret = gdi_init_video(vo, 10, 10, PIX_FMT_YUV420P);
			gdi_destory_render(vo);
			if (ret == 0)
			{
				vo->init_video = gdi_init_video;
				vo->re_size = gdi_re_size;
				vo->aspect_ratio = gdi_aspect_ratio;
				vo->use_overlay = gdi_use_overlay;
				vo->destory_video = gdi_destory_render;
				vo->render_one_frame = gdi_render_one_frame;
				break;
			}
		}
	} while (check-- == 0);

	// ��ʾ��Ƶ��Ⱦ����ʼ��ʧ��!!!
	assert(check != -2);

	// ����thisΪuser_ctx.
}

void init_file_source(source_context *sc)
{
	sc->init_source = file_init_source;
	sc->read_data = file_read_data;
	sc->read_seek = file_read_seek;
	sc->close = file_close;
	sc->destory = file_destory;
}
avplay *m_avplay;
LRESULT CALLBACK static_win_wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (msg == WM_LBUTTONUP)
	{
		av_seek(m_avplay, LOWORD(lparam) / 8 * 0.01);
		
		return 0;
	}
		return DefWindowProc(hwnd, msg, wparam, lparam);
}
int _tmain(int argc, _TCHAR* argv[])
{

	// avplay ������source_context��ao_context,vo_context�Ĵ���.
	// �� source_contextʵ�����ݶ�����Ƶ����.
	// �� avplay����������Ӳ�����.
	// �����ao_context��vo_context�ֱ��������Ƶ����Ƶ.
	
	source_context *m_source;
	ao_context *m_audio;
	vo_context *m_video;

	MSG msg;
	WNDCLASSEX wcex;

	// �õ�����ʵ�����.
	HINSTANCE m_hinstance = (HINSTANCE)GetModuleHandle(NULL);
	// �����Ǵ���ɫ�Ļ�ˢ, ����ddraw����ʱˢ����ɫ.
	HBRUSH m_brbackground = CreateSolidBrush(RGB(0, 0, 1));
	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_CLASSDC/*CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS*/;
	wcex.lpfnWndProc = &static_win_wnd_proc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = m_hinstance;
	wcex.hIcon = LoadIcon(m_hinstance, MAKEINTRESOURCE(IDC_ICON));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = m_brbackground;	// (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName = NULL;				// MAKEINTRESOURCE(IDC_AVPLAYER);
	wcex.lpszClassName = L"aa";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDC_ICON));

	if (!RegisterClassEx(&wcex))
		return NULL;

	printf("�����벥�ŵ�����·��:\n");
	char *aa = (char*)malloc(500);
	scanf("%s", aa);

	// ��������.
	HWND m_hwnd = CreateWindowEx(/*WS_EX_APPWINDOW*/0,
		L"aa" , L"aa", WS_OVERLAPPEDWINDOW/* | WS_CLIPSIBLINGS | WS_CLIPCHILDREN*/,
		0, 0, 1366, 768, NULL, NULL, m_hinstance, NULL);
	
	ShowWindow(m_hwnd, SW_SHOW);

	m_avplay = alloc_avplay_context();


	m_source = alloc_media_source(MEDIA_TYPE_FILE, aa, 6 + 1, 1000000);

	// ��ʼ���ļ�ý��Դ.
	init_file_source(m_source);
	// ��ʼ��avplay.
	initialize(m_avplay, m_source);

	// ������Ƶ����Ƶ����Ⱦ��.
	m_audio = alloc_audio_render();

	m_video = alloc_video_render(m_hwnd);

	// ��ʼ����Ƶ����Ƶ��Ⱦ��.
	init_audio(m_audio);
	init_video(m_video, RENDER_D3D);

	// ������Ƶ��Ƶ��Ⱦ��.
	configure(m_avplay, m_video, VIDEO_RENDER);
	configure(m_avplay, m_audio, AUDIO_RENDER);
	
	av_start(m_avplay, 0, 0);

	enable_calc_frame_rate(m_avplay);
	//�ȴ��̳߳�ʼ���������豸
	Sleep(1000);
	//av_volume(m_avplay, 10, 10);
	

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		
	}
	return 0;
}

