#ifndef __Zui_CONTROLS_PLAYLIST_H__
#define __Zui_CONTROLS_PLAYLIST_H__
// �����б�
#include "../ui/zui.h"

typedef struct _ZPlayList
{
	ZuiText Title;

} *ZuiPlayList, ZPlayList;
/**
* �˺�������������ť.
* @param ParentControl ���ؼ�
* @param Left ���
* @param Top ����
* @param Width ���
* @param Height �߶�
* @param Title ����
* @param StringFormat �ı���ʽ
* @param Visible ����
* @param Enabled ����
* @param Returns �ɹ����ؿؼ����� ʧ�ܷ���NULL
*/
ZAPI(ZuiControl) ZuiPlayListCreate(ZuiControl ParentControl, ZuiInt Left, ZuiInt Top, ZuiInt Width, ZuiInt Height, ZuiBool Visible, ZuiBool Enabled);
#endif	//__Zui_CONTROLS_BUTTON_H__