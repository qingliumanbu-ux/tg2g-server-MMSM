/*************************************************
Copyright:   Baosight Software LTD.co Copyright (c) 2013
Author:      顾云峰
Version:     1.0
Date:        2013-08-21
Description: 按命令板坯号生成目的板坯表和板坯工序表
**************************************************/
/*<remark>============================================================================
/// <summary>
/// 按命令板坯号生成目的板坯表和板坯工序表
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
//#include "tmmsm01.h" 
//#include "tmmsm03.h" 
//#include "tpmouhp30.h"
//#include "tmmsm33"

//外部函数声明
int f_mmsm0003_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);
int f_mmsm0008_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn);

//炼钢计算理论重量函数
int f_mmsm_get_theorywt(CDecimal MAT_ACT_THICK, CDecimal MAT_ACT_WIDTH, CDecimal MAT_ACT_LEN, CDecimal MAT_NUM, CDecimal &MAT_THEORY_WT);

int f_mmsm0005_proc(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
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

	int i_cut_seq = 0;
	int pmouhp30Flag = 0;
	CString v_pono_slab = "";
	long pono_slab_len;
	int i_matWtFlag = 0; //重量标记，当实物板坯的理论重量 > 命令板坯清理前重量之和时，为1 （2011-02-23吴珊修改）

	//使用的表结构变量
	CModel tmmsm01("TMMSM01");
	CModel tmmsm03("TMMSM03");
	CModel tpmouhp30("TPMOUHP30");
	CModel tpssm03("TPSSM03");
	//CTMMSM01 tmmsm01(conn);
 //   CTMMSM03 tmmsm03(conn);
	//CTPMOUHP30 tpmouhp30(conn);

	CString  sqlstr("");
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	
	try
	{
		if(bcls_rec->Tables.Contains("MMSM0005") == false)
		{
			strcpy(s.msg,_RES("GCRSS0000011")/*系统出现异常，数据块有误，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location); 
		}

		if(bcls_rec->Tables.Contains("MMSM0008") == false)
		{
			bcls_rec->Tables.Add("MMSM0008");
			bcls_rec->Tables["MMSM0008"].Columns.Add(DT_STRING,"mat_no");
			bcls_rec->Tables["MMSM0008"].Columns.Add(DT_STRING,"cut_num");
			bcls_rec->Tables["MMSM0008"].Columns.Add(DT_STRING,"cut_seq");
			bcls_rec->Tables["MMSM0008"].Rows.Add();
		}

		if(bcls_rec->Tables.Contains("MMSM0003") == false)
		{
			bcls_rec->Tables.Add("MMSM0003");
		}
		
		//获取传入参数
		tmmsm01["MAT_NO"] = bcls_rec->Tables["MMSM0005"].Rows[0]["mat_no"];
		tmmsm01["FIX_SLAB_NUM"] = bcls_rec->Tables["MMSM0005"].Rows[0]["fix_slab_num"];
		i_cut_seq = bcls_rec->Tables["MMSM0005"].Rows[0]["cut_seq"];
		v_pono_slab = bcls_rec->Tables["MMSM0005"].Rows[0]["pono_slab"];
		i_matWtFlag = bcls_rec->Tables["MMSM0005"].Rows[0]["mat_wt_flag"];

		Log::Trace("",__FUNCTION__,"mat_no = [{0}]",tmmsm01["MAT_NO"].ToString());

		/*校验传入参数*/
		if (tmmsm01["MAT_NO"].ToString().Trim() == "")
		{
			strcpy(s.msg,_RES("GCRSS0000035")/*材料号不能为空。*/);
			throw CApplicationException(-1, s.msg, log.Location); 
		}
		if(tmmsm01["FIX_SLAB_NUM"].ToDecimal() <= 0)
		{
			sprintf(s.msg,_RES("MMHPS0000072")/*数据校验出错，命令板坯块数[{0}]不能小于0。*/,tmmsm01["FIX_SLAB_NUM"]);
			throw CApplicationException(-1, s.msg, log.Location); 
		}
		if(i_cut_seq <= 0)
		{
			sprintf(s.msg,_RES("MMHPS0000082")/*数据校验出错，切割顺序号[{0}]不能为空。*/,i_cut_seq);
			throw CApplicationException(-1, s.msg, log.Location); 
		}

		tmmsm01.Query("MAT_NO");
		
		//命令板坯数>1,则调用板坯号生成程序
		if (tmmsm01["FIX_SLAB_NUM"].ToDecimal() > 1)
		{
			bcls_rec->Tables["MMSM0008"].Rows[0]["mat_no"] = tmmsm01["MAT_NO"];
			bcls_rec->Tables["MMSM0008"].Rows[0]["cut_num"] = tmmsm01["FIX_SLAB_NUM"];
			bcls_rec->Tables["MMSM0008"].Rows[0]["cut_seq"] = i_cut_seq;

			doFlag = f_mmsm0008_proc(bcls_rec, bcls_ret, conn);
			if(doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			tmmsm03["AIM_MAT_NO"] = bcls_rec->Tables["MMSM0008"].Rows[0]["mat_no"];
		}
		else //命令板坯数 =1，材料号＝ 目的板坯号，设计的原因是炼钢板坯产出时，有可能直接产出短尺坯
		{
			tmmsm03["AIM_MAT_NO"] = tmmsm01["MAT_NO"];
		}

		tmmsm03["REC_CREATE_TIME"] = tmmsm01["REC_CREATE_TIME"];
		tmmsm03["REC_CREATOR"] = tmmsm01["REC_CREATOR"];
		tmmsm03["MAT_NO"] = tmmsm01["MAT_NO"];
		tmmsm03["PONO_SLAB"] = v_pono_slab;
		Log::Trace("", __FUNCTION__, "CS = [{0}]", "1111");
		tpssm03["SLAB_NO"] = tmmsm03["PONO_SLAB"];
		Log::Trace("", __FUNCTION__, "CS = [{0}]", "222");
		if (tpssm03.Query("SLAB_NO"))
		{
			pmouhp30Flag = 1;
			//命令板坯表存在
			cmd_inq.SetCommandText(" SELECT ORDER_NO FROM TPSSM03 WHERE SLAB_NO = @tmmsm03.pono_slab ");
			cmd_inq.Parameters.Set("tmmsm03.pono_slab", tmmsm03["PONO_SLAB"]);
			cmd_inq.ExecuteReader();
			if (cmd_inq.Read()) tmmsm03["ORDER_NO"] = cmd_inq.GetString(1);
			cmd_inq.Close();

			//linggu 按谢文学要求将进炉目标长度用清理前长度代替 2015-1-4
			tmmsm03["INFUR_SLAB_LEN"] = tpssm03["SLAB_LEN"]; //tmmsm03.INFUR_SLAB_LEN = tpmouhp30.INFUR_SLAB_LEN;
			if (tpssm03["ORDER_NO"].ToString().Trim() != "")
			{
				tmmsm03["ORDER_REMAIN_DIV"] = "1";
			}
			else
			{
				tmmsm03["ORDER_REMAIN_DIV"] = "0";
			}
			//tmmsm03["ORDER_REMAIN_DIV"] = tpmouhp30["ORDER_REMAIN_DIV"];
			tmmsm03["HOT_CHARGE_FLAG"] = tmmsm01["HOT_CHARGE_FLAG"];
			//dps 安钢定制：将进炉目标厚度和宽度用清理前厚度和宽度代替 2022.4
			tmmsm03["INFUR_SLAB_THICK"] = tpssm03["SLAB_THICK"];
			tmmsm03["INFUR_SLAB_WID"] = tpssm03["SLAB_WIDTH"];

			tmmsm03["INFUR_SLAB_MAX_LEN"] = tpssm03["SLAB_MAX_LEN"]; //tmmsm03.INFUR_SLAB_MAX_LEN = tpmouhp30.INFUR_SLAB_MAX_LEN;
			tmmsm03["INFUR_SLAB_MIN_LEN"] = tpssm03["SLAB_MIN_LEN"]; //tmmsm03.INFUR_SLAB_MIN_LEN = tpmouhp30.INFUR_SLAB_MIN_LEN;//tpmouhp30.NEED_SLAB_LEN;

			//dps 安钢定制：将进加热炉重量调用炼钢函数进行重新计算
			CDecimal slabWt;
			if (f_mmsm_get_theorywt(tmmsm03["INFUR_SLAB_THICK"].ToDecimal(), tmmsm03["INFUR_SLAB_WID"].ToDecimal(), tmmsm03["INFUR_SLAB_LEN"].ToDecimal(), 1, slabWt) < 0){
				strcpy(s.msg, "调用炼钢函数计算理论重量失败");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			CDecimal slabMaxWt;
			if (f_mmsm_get_theorywt(tmmsm03["INFUR_SLAB_THICK"].ToDecimal(), tmmsm03["INFUR_SLAB_WID"].ToDecimal(), tmmsm03["INFUR_SLAB_MAX_LEN"].ToDecimal(), 1, slabMaxWt) < 0){
				strcpy(s.msg, "调用炼钢函数计算理论重量失败");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			CDecimal slabMinWt;
			if (f_mmsm_get_theorywt(tmmsm03["INFUR_SLAB_THICK"].ToDecimal(), tmmsm03["INFUR_SLAB_WID"].ToDecimal(), tmmsm03["INFUR_SLAB_MIN_LEN"].ToDecimal(), 1, slabMinWt) < 0){
				strcpy(s.msg, "调用炼钢函数计算理论重量失败");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			tmmsm03["INFUR_SLAB_WT"] = slabWt;
			tmmsm03["INFUR_SLAB_MIN_WT"] = slabMinWt;// tpmouhp30.NEED_SLAB_WT;
			tmmsm03["INFUR_SLAB_MAX_WT"] = slabMaxWt;
		}
		else
		{
			tpmouhp30["PONO_SLAB"] = tmmsm03["PONO_SLAB"];
			if (tpmouhp30.Query("PONO_SLAB"))
			{
				pmouhp30Flag = 1;
				//命令板坯表存在
				cmd_inq.SetCommandText(" SELECT ORDER_NO FROM TPMOUHP32 WHERE PONO_SLAB = @tmmsm03.pono_slab AND ORDER_SEQ_NO = 1 ");
				cmd_inq.Parameters.Set("tmmsm03.pono_slab", tmmsm03["PONO_SLAB"]);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read()) tmmsm03["ORDER_NO"] = cmd_inq.GetString(1);
				cmd_inq.Close();

				//linggu 按谢文学要求将进炉目标长度用清理前长度代替 2015-1-4
				tmmsm03["INFUR_SLAB_LEN"] = tpmouhp30["PRE_CLEAN_SLAB_LEN"]; //tmmsm03.INFUR_SLAB_LEN = tpmouhp30.INFUR_SLAB_LEN;
				tmmsm03["ORDER_REMAIN_DIV"] = tpmouhp30["ORDER_REMAIN_DIV"];
				tmmsm03["HOT_CHARGE_FLAG"] = tmmsm01["HOT_CHARGE_FLAG"];
				//dps 安钢定制：将进炉目标厚度和宽度用清理前厚度和宽度代替 2022.4
				tmmsm03["INFUR_SLAB_THICK"] = tpmouhp30["PRE_CLEAN_SLAB_THICK"];
				tmmsm03["INFUR_SLAB_WID"] = tpmouhp30["PRE_CLEAN_SLAB_WIDTH"];

				tmmsm03["INFUR_SLAB_MAX_LEN"] = tpmouhp30["PRE_CLEAN_SLAB_MAX_LEN"]; //tmmsm03.INFUR_SLAB_MAX_LEN = tpmouhp30.INFUR_SLAB_MAX_LEN;
				tmmsm03["INFUR_SLAB_MIN_LEN"] = tpmouhp30["PRE_CLEAN_SLAB_MIN_LEN"]; //tmmsm03.INFUR_SLAB_MIN_LEN = tpmouhp30.INFUR_SLAB_MIN_LEN;//tpmouhp30.NEED_SLAB_LEN;

				//dps 安钢定制：将进加热炉重量调用炼钢函数进行重新计算
				CDecimal slabWt;
				if (f_mmsm_get_theorywt(tmmsm03["INFUR_SLAB_THICK"].ToDecimal(), tmmsm03["INFUR_SLAB_WID"].ToDecimal(), tmmsm03["INFUR_SLAB_LEN"].ToDecimal(), 1, slabWt) < 0){
					strcpy(s.msg, "调用炼钢函数计算理论重量失败");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				CDecimal slabMaxWt;
				if (f_mmsm_get_theorywt(tmmsm03["INFUR_SLAB_THICK"].ToDecimal(), tmmsm03["INFUR_SLAB_WID"].ToDecimal(), tmmsm03["INFUR_SLAB_MAX_LEN"].ToDecimal(), 1, slabMaxWt) < 0){
					strcpy(s.msg, "调用炼钢函数计算理论重量失败");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
				CDecimal slabMinWt;
				if (f_mmsm_get_theorywt(tmmsm03["INFUR_SLAB_THICK"].ToDecimal(), tmmsm03["INFUR_SLAB_WID"].ToDecimal(), tmmsm03["INFUR_SLAB_MIN_LEN"].ToDecimal(), 1, slabMinWt) < 0){
					strcpy(s.msg, "调用炼钢函数计算理论重量失败");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				tmmsm03["INFUR_SLAB_WT"] = slabWt;
				tmmsm03["INFUR_SLAB_MIN_WT"] = slabMinWt;// tpmouhp30.NEED_SLAB_WT;
				tmmsm03["INFUR_SLAB_MAX_WT"] = slabMaxWt;
			}
			
		}
		
		tmmsm03.Print();

		tmmsm03.Insert();

		Log::Trace("",__FUNCTION__,"调用f_MMSM0003_proc生成板坯工序,目的板坯号 = [{0}]",tmmsm03["AIM_MAT_NO"].ToString());

		if(pmouhp30Flag == 1)
		{
			bcls_rec->Tables["MMSM0003"].Rows.Clear();
			tmmsm03.MergeTo(bcls_rec->Tables["MMSM0003"],false);

			//doFlag = f_mmsm0003_proc(bcls_rec, bcls_ret,conn);
			if(doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
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