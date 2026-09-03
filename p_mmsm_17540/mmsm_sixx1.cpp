/// <summary>
/// 功能说明: 生产模块画面布局配置获取
/// </summary>
/// Copyright: Baosight Software LTD.co Copyright (c) 2010
/// Company: 上海宝信软件股份有限公司
/// Author:   项目组
/// Version:  1.0
/// History:  2022/7/28 17:27:16
///          
///	

#include "stdafx.h"
//#include "Be2UserModel/SI/CFormDevConfig.h"
#include "Be2UserModel/SI/si00_common.h"

// Service 入口
BM2F_ENTERACE(mmsm_sixx1)
int f_mmsm_sixx1(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	int doFlag = 0;

	try
	{
		EDLog(1, 1, "==================== si00_form_get Begin ====================");

		doFlag = BE2::CFormDevConfig::GetFormDevConfig(bcls_rec, bcls_ret, conn);

		if (doFlag != 0)
		{
			s.flag = -1;
			return -1;
		}

		EDLog(1, 1, "==================== si00_form_get End ====================");
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		EDLog(1, 1, "s.msg = [%s]", s.msg);
		s.flag = -1;
		doFlag = -1;
	}
	return doFlag;
}
