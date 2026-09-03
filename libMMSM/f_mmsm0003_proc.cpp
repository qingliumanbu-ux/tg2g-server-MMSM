/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2013
Author:      顾云峰
Version:     1.0
Date:        2013-08-21
Description: 按命令板坯号生成板坯小工序表
**************************************************/
/*<remark>============================================================================
/// <summary>
/// 按命令板坯号生成板坯小工序表
/// <para>
/// </summary>
/// <param name="">                                                           </param>
/// <returns>                                                               </returns>
============================================================================</remark>*/
/***** C/C++ 的标准头文件部分 *****/
#include "stdafx.h"
using namespace BM2;
using namespace BM2::Data;
using namespace BM2::Data::DbClient; 

#include "EI_TUXClass.h"

/* ***** 程序表结构引用 ***** */
//#include "tmmsm03.h" 
//#include "tmmsm04.h" 
//#include "tmmsm01.h"
//#include "tpmouhp30.h" 

//外部函数声明

int f_mmsm0003_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	//应用处理开始
	CTracer log(__FUNCTION__);

	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	int atFlag = 0;
	int i;
	int blkNum;
	int fetchRowCount;
	int i_count = 0;
	CString  datetime = "";
	
	int v_passSubBacklogSeq = 0;
	int v_count = 0;
	int i_col_index = 0;

	//使用的表结构变量
	CModel tmmsm01("TMMSM01");
	CModel tmmsm03("TMMSM03");
	CModel tmmsm04("TMMSM04");
	CModel tpmouhp30("TPMOUHP30");

	//CTMMSM01 tmmsm01(conn);
	//CTMMSM03 tmmsm03(conn);
 //   CTMMSM04 tmmsm04(conn);
	//CTPMOUHP30	tpmouhp30(conn);

	CString  sqlstr("");
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq2(conn);
	
	try
	{
		if(bcls_rec->Tables.Contains("MMSM0003") == false)
		{
			strcpy(s.msg,_RES("GCRSS0000011")/*系统出现异常，数据块有误，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location); 
		}
		
		//获取传入参数
		tmmsm03.MergeFrom(bcls_rec->Tables["MMSM0003"].Rows[0]);
		//Log::Trace("",__FUNCTION__,"mat_no = [{0}]",tmmsm03["MAT_NO"]);

		/*校验传入参数*/
		if (tmmsm03["MAT_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg,_RES("GCRSS0000035")/*材料号不能为空。*/);
			throw CApplicationException(-1, s.msg, log.Location); 
		}
		tmmsm01["MAT_NO"] = tmmsm03["MAT_NO"];
		tmmsm01.Query();
		
		tpmouhp30["PONO_SLAB"] = tmmsm03["PONO_SLAB"];
		tpmouhp30.Query("PONO_SLAB");
		//EDIT NVL SQL BY ZHAOLIYUAN 20200411
		cmd_inq.SetCommandText(" SELECT NVL(MAX(SUB_BACKLOG_SEQ),0) FROM TMMSM04 WHERE MAT_NO = @tmmsm03.mat_no AND BACKLOG_PASS_TIME > ' ' ");
		cmd_inq.Parameters.Set("tmmsm03.mat_no",tmmsm03["MAT_NO"]);
		v_passSubBacklogSeq = cmd_inq.ExecuteScalar().ToInt32();
		cmd_inq.Close();

		if(v_passSubBacklogSeq == 0)
			v_passSubBacklogSeq = 1;
		else v_passSubBacklogSeq = v_passSubBacklogSeq + 1;
		
		cmd_inq.SetCommandText(" SELECT * FROM TMMSM04 WHERE MAT_NO = @tmmsm03.mat_no AND BACKLOG_PASS_TIME > ' ' AND AIM_MAT_NO = (SELECT NVL(MIN(AIM_MAT_NO),' ') FROM TMMSM04 WHERE MAT_NO = @tmmsm03.mat_no) ORDER BY SUB_BACKLOG_SEQ ");
		cmd_inq.Parameters.Set("tmmsm03.mat_no",tmmsm03["MAT_NO"]);
		cmd_inq.ExecuteReader();
		while(cmd_inq.Read())
		{
			cmd_inq.Fetch(tmmsm04);

			cmd_inq2.SetCommandText(" SELECT COUNT(0) FROM TMMSM04 WHERE AIM_MAT_NO = @tmmsm03.aim_mat_no AND SUB_BACKLOG_SEQ = @tmmsm04.sub_backlog_seq ");
			cmd_inq2.Parameters.Set("tmmsm03.aim_mat_no",tmmsm03["AIM_MAT_NO"]);
			cmd_inq2.Parameters.Set("tmmsm04.sub_backlog_seq",tmmsm04["SUB_BACKLOG_SEQ"]);
			v_count = cmd_inq2.ExecuteScalar().ToInt32();
			cmd_inq2.Close();

			if(v_count == 0)
			{
				tmmsm04["REC_CREATE_TIME"] = tmmsm01["REC_CREATE_TIME"];
				tmmsm04["REC_CREATOR"] = tmmsm01["REC_CREATOR"];
				tmmsm04["AIM_MAT_NO"] = tmmsm03["AIM_MAT_NO"];
				tmmsm04["PONO_SLAB"] = tmmsm03["PONO_SLAB"];
				tmmsm04["MAT_NO"] = tmmsm03["MAT_NO"];

				tmmsm04.Insert();
			}
		}
		cmd_inq.Close();

		Log::Trace("",__FUNCTION__,"linggu 已过工序序号 [{0}]",v_passSubBacklogSeq);
		//按产销0003程序修改 ADD BY CS 20200309
		int secut_flag = 0;
		/* 按命令板坯表_厚板的板坯预定通过工序 新增板坯工序表 */
		const CFieldInfoCollection& tpmouhp30_fieldInfoCollection = tpmouhp30.GetFields();
		for (int i = 0; i < tpmouhp30_fieldInfoCollection.get_Count(); i++)
		{
			//对tpmouhp30的每个字段进行分解
			const CFieldInfo& tpmouhp30_fieldInfo = tpmouhp30_fieldInfoCollection[i];

			//字段名是 板坯预定通过工序N 并且值不为空,则新增板坯工序表
			if (tpmouhp30_fieldInfo.ColumnName.Find("SLAB_PRE_PROC") >= 0)
			{
				if (*(CString*)tpmouhp30_fieldInfo.Value != " ")
				{
					tmmsm04["SUB_BACKLOG_CODE"] = *(CString*)tpmouhp30_fieldInfo.Value;

					if (tmmsm04["SUB_BACKLOG_CODE"].ToString().Substring(1, 1) == "B")
					{
						//代表命令中有二切
						secut_flag = 1;
						break;
					}
				}
			}
		}
		//按30表生成板坯工序
		for (int i = 0; i < tpmouhp30_fieldInfoCollection.get_Count(); i++)
		{
			//对tpmouhp30的每个字段进行分解
			const CFieldInfo& tpmouhp30_fieldInfo = tpmouhp30_fieldInfoCollection[i];

			//字段名是 板坯预定通过工序N 并且值不为空,则新增板坯工序表
			if (tpmouhp30_fieldInfo.ColumnName.Find("SLAB_PRE_PROC") >= 0)
			{
				if (*(CString*)tpmouhp30_fieldInfo.Value != " ")
				{
					tmmsm04["AIM_MAT_NO"] = tmmsm03["AIM_MAT_NO"];
					tmmsm04["SUB_BACKLOG_SEQ"] = v_passSubBacklogSeq;
					tmmsm04["SUB_BACKLOG_CODE"] = *(CString*)tpmouhp30_fieldInfo.Value;
					tmmsm04["BACKLOG_ADD_DIV"] = "0";
					tmmsm04["BACKLOG_ADD_CAUSE_CODE"] = "";
					tmmsm04["ACT_SUB_BACKLOG_CODE"] = "";
					tmmsm04["BACKLOG_PASS_TIME"] = "";
					tmmsm04["BACKLOG_DECIDE_CODE"] = "";
					tmmsm04["PONO_SLAB"] = tmmsm03["PONO_SLAB"];
					tmmsm04["MAT_NO"] = tmmsm03["MAT_NO"];

					//Log::Trace("", __FUNCTION__, "新增板坯工序表 tmmsm04.SUB_BACKLOG_CODE	= [{0}]", tmmsm04["SUB_BACKLOG_CODE"]);
					//Log::Trace("", __FUNCTION__, "新增板坯工序表 tmmsm04.SUB_BACKLOG_SEQ	= [{0}]", tmmsm04["SUB_BACKLOG_SEQ"]);
					//Log::Trace("", __FUNCTION__, "linggu secut_flag	= [{0}]", secut_flag);
					//Log::Trace("", __FUNCTION__, "linggu FIX_SLAB_NUM	= [{0}]", tmmsm01["FIX_SLAB_NUM"]);
					//Log::Trace("", __FUNCTION__, "linggu trace 0003 ");

					//如果预定中有二切工序，但目前只是短坯了，则二切工序跳过
					if (tmmsm01["FIX_SLAB_NUM"].ToDecimal() <= 1 && secut_flag > 0 && tmmsm04["SUB_BACKLOG_CODE"].ToString().Substring(1, 1) == "B")
					{
						Log::Trace("", __FUNCTION__, "linggu 计划中有二切，短坯跳过 ");
						continue;
					}

					//LINGGU
					//如果预定中没有二切工序，但又是长坯，则自动在轧制前生成二切工序
					if (tmmsm01["FIX_SLAB_NUM"].ToDecimal() > 1 && secut_flag == 0 && tmmsm04["SUB_BACKLOG_CODE"].ToString().Substring(1, 1) == "R")
					{
						Log::Trace("", __FUNCTION__, "linggu 计划中无二切，长坯自动追加 ");

						//原tmmsm04.SUB_BACKLOG_CODE = tmmsm04.SUB_BACKLOG_CODE.Substring(0, 1) + "B1"; //默认按轧制产线生成二切
						//20200311修改by zxx
						tmmsm04["SUB_BACKLOG_CODE"] = "PB1";
						tmmsm04["REC_CREATE_TIME"] = datetime;
						tmmsm04["REC_CREATOR"] = s.userid;
						tmmsm04.TrimOrBlank();
						tmmsm04.Insert();

						v_passSubBacklogSeq = v_passSubBacklogSeq + 1;

						//轧制工序序号后移
						tmmsm04["SUB_BACKLOG_SEQ"] = v_passSubBacklogSeq;
						tmmsm04["SUB_BACKLOG_CODE"] = *(CString*)tpmouhp30_fieldInfo.Value;
					}

					/* 新增板坯工序表 */
					tmmsm04["REC_CREATE_TIME"] = datetime;
					tmmsm04["REC_CREATOR"] = s.userid;
					tmmsm04.TrimOrBlank();
					tmmsm04.Insert();

					v_passSubBacklogSeq = v_passSubBacklogSeq + 1;
				}
			}
		}




	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;

}