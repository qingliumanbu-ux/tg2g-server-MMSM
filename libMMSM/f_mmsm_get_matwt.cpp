/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-07-04
Description: 获取材料重量
remark:本
***********************************************************************/
/***** C/C++ 的标准头文件部分 *****/ 
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
/***** C++ 的业务头文件部分 *****/ 




//外部函数声明
BM2_FUNCTION_EXPORT
int f_mmsm_get_theorywt( CDecimal MAT_ACT_THICK, CDecimal MAT_ACT_WIDTH, CDecimal MAT_ACT_LEN, CDecimal MAT_NUM, CDecimal &MAT_THEORY_WT);

int f_mmsm_get_matwt(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
		/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm_get_matwt";                //定义函数英文名称  
	CString FunctionCname = "获取材料重量";              //定义函数中文名称
	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义
 
	/****** 自定义变量 ***** */
	int doFlag = 0;
	int ret = 0;
	int blkNum = 0;
	CString c_datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");          //当前时间
	CString v_func_id = "";
	CString v_lslab_no = "";
	CString v_slab_no = "";
	CString v_ingot_code = "";
	CDecimal v_slab_len = 0;             
	CDecimal v_slab_wt = 0;       
	CDecimal v_slab_num = 1;
	CDecimal t_slab_wt = 0;
	int fetchRowCount = 0;
	CString sqlstr = "";
	CDecimal mat_radius;
	const CDecimal density = 7.85;
	

	CDbCommand cmd_inq(conn);
	CModel tmmsm33("TMMSM33");
	CModel tmmsmwt("TMMSMWT");
	CModel tmmsmwta("TMMSMWTA");

	try
	{
		//Log::Trace("", "", "获取传入参数");
		for (int i = 0; i < bcls_rec->Tables["TMMSM33"].Rows.get_Count(); i++)
		{
			tmmsm33.Reset();
			tmmsm33.MergeFrom(bcls_rec->Tables["TMMSM33"].Rows[i]);
			tmmsm33.TrimOrBlank();

			//Log::Trace("", "", "tmmsm33["MANAGE_FLAG"] ={0}", tmmsm33["MANAGE_FLAG"].ToString());
			Log::Trace("", "", "tmmsm33.SLAB_TYPE ={0}", tmmsm33["SLAB_TYPE"].ToString());
			//Log::Trace("", "", "tmmsm33["INGOT_CODE"] ={0}", tmmsm33["INGOT_CODE"].ToString());


			//Log::Trace("", "", "获取传入参数");

			if (tmmsm33["SLAB_TYPE"].ToString() == "5") //模铸
			{
				cmd_inq.SetCommandText("SELECT RESUME_SEQ_NO,WT_PER_METER FROM TMMSMWT WHERE RESUME_SEQ_NO = (SELECT MAX(RESUME_SEQ_NO) FROM TMMSMWT WHERE  INGOT_CODE = @tmmsm33.INGOT_CODE)");
			}
			else
			{
				Log::Trace("", "", "tmmsm33.STATION_NO ={0}", tmmsm33["STATION_NO"].ToString());
				if (tmmsm33["STATION_NO"].ToString().Trim() != "")
				{
					/*cmd_inq.SetCommandText("SELECT RESUME_SEQ_NO,WT_PER_METER FROM TMMSMWT WHERE RESUME_SEQ_NO = (SELECT MAX(RESUME_SEQ_NO) FROM TMMSMWT WHERE CC_MACH_NO = @tmmsm33.STATION_NO AND SLAB_THICK = @tmmsm33.SLAB_THICK AND SLAB_WIDTH = @tmmsm33.LAB_WIDTH)");*/
					cmd_inq.SetCommandText("SELECT RESUME_SEQ_NO,WT_PER_METER FROM TMMSMWT WHERE RESUME_SEQ_NO = (SELECT MAX(RESUME_SEQ_NO) FROM TMMSMWT WHERE SLAB_THICK = @tmmsm33.SLAB_THICK AND SLAB_WIDTH = @tmmsm33.SLAB_WIDTH)");
				}
				else
				{
					cmd_inq.SetCommandText("SELECT RESUME_SEQ_NO,WT_PER_METER FROM TMMSMWT WHERE RESUME_SEQ_NO = (SELECT MAX(RESUME_SEQ_NO) FROM TMMSMWT WHERE SLAB_THICK = @tmmsm33.SLAB_THICK AND SLAB_WIDTH = @tmmsm33.SLAB_WIDTH)");
				}
			}



			cmd_inq.Parameters.Set("tmmsm33.STATION_NO", tmmsm33["STATION_NO"].ToString());
			cmd_inq.Parameters.Set("tmmsm33.SLAB_THICK", tmmsm33["SLAB_THICK"].ToDecimal());
			cmd_inq.Parameters.Set("tmmsm33.SLAB_WIDTH", tmmsm33["SLAB_WIDTH"].ToDecimal());
			cmd_inq.Parameters.Set("tmmsm33.INGOT_CODE", tmmsm33["INGOT_CODE"].ToString());
			cmd_inq.ExecuteReader();
			Log::Trace("", "", "开始读称重标定表");
			if (cmd_inq.Read())
			{

				tmmsmwt["WT_PER_METER"] = cmd_inq.GetDecimal(2);

				//Log::Trace("", "", "tmmsmwt.WT_PER_METER11111= {0}", tmmsmwt["WT_PER_METER"].ToDecimal());

				if (tmmsm33["SLAB_TYPE"].ToString() == "1") //板坯
				{
					tmmsm33["SLAB_WT"] = (tmmsm33["SLAB_LEN"].ToDecimal() / 1000) * tmmsmwt["WT_PER_METER"];
				}
				else if (tmmsm33["SLAB_TYPE"].ToString() == "3") //方坯
				{
					tmmsm33["SLAB_WT"] = (tmmsm33["SLAB_LEN"].ToDecimal() / 1000) * tmmsmwt["WT_PER_METER"].ToDecimal() * tmmsm33["MAT_TUBE"];;
				}
				else if (tmmsm33["SLAB_TYPE"].ToString() == "4") //圆坯
				{

					tmmsm33["SLAB_WT"] = (tmmsm33["SLAB_LEN"].ToDecimal() / 1000) * tmmsmwt["WT_PER_METER"];
				}
				else if (tmmsm33["SLAB_TYPE"].ToString() == "5") //模铸
				{
					tmmsm33["SLAB_WT"] = tmmsmwt["WT_PER_METER"];
				}

				tmmsm33["SLAB_WT"] = tmmsm33["SLAB_WT"].ToDecimal().Round(3);
				if (tmmsm33["MAT_NO"].ToString().Trim() != "")
				{
					tmmsmwta["REC_CREATE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
					tmmsmwta["REC_CREATOR"] = s.userid;
					tmmsmwta["RESUME_SEQ_NO"] = cmd_inq.GetString(1);
					tmmsmwta["MAT_NO"] = tmmsm33["MAT_NO"];
					tmmsmwta["WT_PER_METER"] = tmmsmwt["WT_PER_METER"];
					tmmsmwta.Insert();
				}

			}
			else
			{
				tmmsm33["SLAB_WT"] = 0;
				if (tmmsm33["SLAB_WIDTH"].ToDecimal() == 0) //圆坯
				{
					mat_radius = tmmsm33["SLAB_THICK"].ToDecimal() / 2000;
					tmmsm33["SLAB_WT"] = 3.14* mat_radius * mat_radius *  (tmmsm33["SLAB_LEN"].ToDecimal() / 1000) * density * tmmsm33["MAT_TUBE"];
				}
				else
				{
					tmmsm33["SLAB_WT"] = (tmmsm33["SLAB_THICK"].ToDecimal() / 1000) * (tmmsm33["SLAB_WIDTH"].ToDecimal() / 1000) * (tmmsm33["SLAB_LEN"].ToDecimal() / 1000) * density * tmmsm33["MAT_TUBE"];
				}
				
			}
			cmd_inq.Close();
			Log::Trace("", "", "重量tmmsm33.SLAB_WT = {0}", tmmsm33["SLAB_WT"].ToDecimal());

			if (tmmsm33["SLAB_WT"].ToDecimal() <= 0)
			{
				ret = f_mmsm_get_theorywt(tmmsm33["SLAB_THICK"].ToDecimal(), tmmsm33["SLAB_WIDTH"].ToDecimal(), tmmsm33["SLAB_LEN"].ToDecimal(), tmmsm33["MAT_TUBE"].ToDecimal(), v_slab_wt);
				if (ret < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				//Log::Trace("", "", "理论重量tmmsm33["SLAB_WT"] = {0}", tmmsm33["SLAB_WT"].ToDecimal());
				tmmsm33["SLAB_WT"] = v_slab_wt;
			}
			tmmsm33["MEASURE_WT_FLAG"] = "0";
			/*else
			{
			tmmsm33["MEASURE_WT_FLAG"] = "2";
			}*/
			bcls_rec->Tables["TMMSM33"].Rows[i]["MEASURE_WT_FLAG"] = tmmsm33["MEASURE_WT_FLAG"];
			bcls_rec->Tables["TMMSM33"].Rows[i]["SLAB_WT"] = tmmsm33["SLAB_WT"].ToDecimal().Round(3);
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
	if (doFlag < 0)
	{
		;
	}
	return doFlag;

}
