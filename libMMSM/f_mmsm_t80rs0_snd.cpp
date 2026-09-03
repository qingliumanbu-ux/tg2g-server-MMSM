/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      李振
Version:     1.0
Date:        2023-11-20
Description: 物料事件同步
**************************************************/

#include "stdafx.h"
#include "epex.h"

BM2_FUNCTION_EXPORT


int f_mmsm_t80rs0_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	int blkNum = 0;
	CString sqlstr = " ";
	CString v_guide_dest = " ";//指导去向
	CString v_guide_dest_snd = " ";//发送指导去向
	CDbCommand cmd_inq(conn);
	//电文号
	CString cs_tc_no("");
	//电文变量
	EPEX epex(&s, conn);
	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");

	try
	{
		CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		Log::Trace("", "", "", "[{0}]", bcls_rec->Tables["NEWMM_TABLE"].Rows.get_Count());
		for (int i=0; i < bcls_rec->Tables["NEWMM_TABLE"].Rows.get_Count(); i++) {
			if (bcls_rec->Tables["NEWMM_TABLE"].Rows[i]["KEYVALUE_3"].ToString() != "1")
			{
				continue;
			}
			/* 判断是否存在指定块 */

			cs_tc_no = "T800S0";
			//电文初始化
			
			tmmsm96.Reset();
			tmmsm96.MergeFrom(bcls_rec->Tables["NEWMM_TABLE"].Rows[i]);
			tmmsm01.MergeFrom(bcls_rec->Tables["NEWMM_TABLE"].Rows[i]);
			tmmsm01.Query("MAT_NO");
			if (tmmsm01["RCV_MAT_FLAG"].ToString() == "S")
			{
				if (epex.Initialize(cs_tc_no) < 0)
				{
					strncpy(s.msg, (const char*)"电文初始化失败", sizeof(s.msg) - 1);
					s.flag = -1;
					doFlag = -1;
					return doFlag;
				}

				//2026.03.25 指导去向发送小代码对应文字
				if (tmmsm96["GUIDE_DEST"].ToString().Trim() != "")
				{
					v_guide_dest = tmmsm96["GUIDE_DEST"].ToString().Trim();
					sqlstr = " SELECT CODE_DESC_1_CONTENT FROM TWMSMZD02 T WHERE 1=1 AND T.CODE_CLASS='WM02' AND T.CODE = @v_guide_dest ";

					cmd_inq.SetCommandText(sqlstr);
					cmd_inq.Parameters.Set("v_guide_dest", v_guide_dest);
					cmd_inq.ExecuteReader();
					if (cmd_inq.Read())
					{
						v_guide_dest_snd = cmd_inq.GetString(1);
					}
					cmd_inq.Close();
					Log::Trace("", __FUNCTION__, "v_guide_dest_snd[{0}]  ", v_guide_dest_snd);
				}

				//2026.03.19 借用命令板坯号10发送指导去向
				tmmsm96["PONO_SLAB_10"] = v_guide_dest_snd;

				if (epex.SetValue(0, tmmsm96) < 0)
				{
					sprintf(s.msg, "发送电文失败，原因[%s]", epex.GetMsg());
					throw CApplicationException(-1, s.msg, s.svc_name);
				}



				//电文发送
				if (epex.SendTele() < 0)
				{
					strncpy(s.msg, (const char*)"电文发送失败", sizeof(s.msg) - 1);
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				else
				{
					Log::Trace("", __FUNCTION__, "发送电文成功");
				}
				epex.Uninitialize();
			}

			
		}

		

	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


