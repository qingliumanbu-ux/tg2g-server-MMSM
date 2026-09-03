/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2014-07-04
Description: 板坯切断实绩修改
***********************************************************************/
/***** C/C++ 的标准头文件部分 *****/ 
// New Include

//框架公用头文件，勿删
#include "stdafx.h"
//程序用头文件


/***** C++ 的业务头文件部分 *****/ 





//连铸铸坯产出处理函数

int f_mmsm3301u_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

//获取重量
int f_mmsm_get_matwt(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);


//外部函数声明

BM2_FUNCTION_EXPORT
 int f_mmsm33_update(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
{
	/****** 定义函数名称 ***** */
	CString FunctionEname = "f_mmsm33_update";                //定义函数英文名称  
	CString FunctionCname = "板坯切断_信息修改";              //定义函数中文名称


	CTracer log(__FUNCTION__);  // 系统日志，必须在代码段开始处定义
	  

	/* ***** 自定义变量 ***** */
	int  doFlag = 0;
	int  ret = 0;
	int  n_count = 0;
	int  blkNum = 0;

	CString c_factory_div = ""; //厂别区分
	CString c_datetime = "";          //当前时间
	CString c_heat_confm_flag = "";   //炉次确定标志
	CString updateColumns = "";

	CString sqlstr = "";
	CModel tmmsm33("TMMSM33");
	CModel hmmsm33("TMMSM33");
	CModel tmmsm01("TMMSM01");

	CDbCommand cmd_inq(conn);

	try
	{
		EDLog(1, 1, "********************获取传入数据开始*******************");
		tmmsm33.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		//tmmsm33.MergeFrom(bcls_rec->Tables[1].Rows[0]);   //信融端目前只传一个TABLE  mfj  20231207
		EDLog(1, 1, "********************获取传入数据结束*******************");

		if (!bcls_rec->Tables.Contains("TMMSM33")){
			bcls_rec->Tables[0].set_TableName("TMMSM33");
		}
		Log::Trace("", __FUNCTION__, "LIKE=[{0}]", __LINE__);
		/*tmmsm33["MAT_NO"] = bcls_rec->Tables[1].Rows[0]["MAT_NO"].ToString().Trim();*/
		hmmsm33["MAT_NO"] = tmmsm33["MAT_NO"];
		hmmsm33.Query("MAT_NO");

		//这里做处理或97A1改事件字段都可以   mfj  20231207
		if (tmmsm33["IF_TRANSFER"].ToString().Trim() == "")
		{
			tmmsm33["IF_TRANSFER"] = "0";
		}

		//tmmsm33.Print();

		tmmsm01["MAT_NO"] = tmmsm33["MAT_NO"];

		//Log::Trace("", "", "tmmsm33["MAT_NO"] ={0}", tmmsm33["MAT_NO"].ToString());
		Log::Trace("", "", "tmmsm01.MAT_NO ={0}", tmmsm01["MAT_NO"].ToString());

		if (!tmmsm01.Query("MAT_NO"))
		{
			CFormattable arguments[] = { (const char*)tmmsm01["MAT_NO"].ToString() };// 定义参数列表的数组
			CMessageFormat::Format(s.msg, "材料号{0}在主档中不存在", arguments, 1);//格式化字符串
			strcpy(s.sysmsg, s.msg);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tmmsm01["MAT_LINE_TYPE"].ToString().Trim() != "SM")
		{
			CFormattable arguments[] = { (const char*)tmmsm01["MAT_NO"].ToString(), (const char*)tmmsm01["MAT_LINE_TYPE"].ToString() };// 定义参数列表的数组
			CMessageFormat::Format(s.msg, "材料号{0}信息产线已变为{1}，已不在炼钢厂，可能已在轧钢厂确认，请联系轧钢原料人员进行退库", arguments, 2);//格式化字符串
			strcpy(s.sysmsg, s.msg);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		/* ***** 打印输入参数 ***** */
		/*EDLog(1, 1, "********************输出传入数据开始*******************");
		tmmsm33.Print();
		EDLog(1, 1, "********************输出传入数据结束*******************");*/

		/* ***** 打印输入参数 ***** */
		if (tmmsm33["MAT_NO"].ToString().Trim() == "")
		{
			strcpy(s.sysmsg, _RES("GCRSS0000035")/*"材料号不能为空!"*/);
			strcpy(s.msg, _RES("GCRSS0000035")/*"材料号不能为空!"*/); //用户提示信息，国际化
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tmmsm33["PONO"].ToString() == "")
		{
			strcpy(s.sysmsg, _RES("MM00S0000072")/*"制造命令号不能为空!"*/);
			strcpy(s.msg, _RES("MM00S0000072")/*"制造命令号不能为空!"*/); //用户提示信息，国际化
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tmmsm33["HEAT_NO"].ToString() == "")
		{
			strcpy(s.sysmsg, "熔炼号不能为空!");
			strcpy(s.msg, s.sysmsg); //用户提示信息，国际化
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (tmmsm33["SLAB_CUT_TIME"].ToString() == "")
		{
			CFormattable arguments[] = { (const char*)tmmsm01["MAT_NO"].ToString() };// 定义参数列表的数组
			CMessageFormat::Format(s.msg, _RES("MMSMS0000004")/*材料号[{0}]切断时刻不能为空。*/, arguments, 1);//格式化字符串
			strcpy(s.sysmsg, s.msg);
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if ((tmmsm33["PROD_SHIFT_NO"].ToString().Trim() == "" || tmmsm33["PROD_SHIFT_GROUP"].ToString().Trim() == "") && (tmmsm33["SLAB_CUT_TIME"].ToString().Trim() != ""))
		{
			CString PROD_SHIFT_NO = "";
			CString PROD_SHIFT_GROUP = "";
			f_epep_get_shift_group("SM", tmmsm33["SLAB_CUT_TIME"].ToString(), PROD_SHIFT_NO, PROD_SHIFT_GROUP, conn);
			tmmsm33["PROD_SHIFT_NO"] = PROD_SHIFT_NO;
			tmmsm33["PROD_SHIFT_GROUP"] = PROD_SHIFT_GROUP;
			//Log::Trace("", __FUNCTION__, "tmmsm33["SLAB_CUT_TIME"] ={0},tmmsm33["PROD_SHIFT_NO"] ={1},tmmsm33["PROD_SHIFT_GROUP"] ={2}", tmmsm33["SLAB_CUT_TIME"].ToString(), tmmsm33["PROD_SHIFT_NO"].ToString(), tmmsm33["PROD_SHIFT_GROUP"].ToString());
		}

		//Log::Trace("", "", "tmmsm33.SLAB_TYPE11={0}", tmmsm33["SLAB_TYPE"].ToString());

		if (tmmsm33["SLAB_TYPE"].ToString() == "4")//圆坯
		{
			if (tmmsm33["SLAB_THICK"].ToDecimal() <= 0 || tmmsm33["SLAB_LEN"].ToDecimal() <= 0 || tmmsm33["MAT_TUBE"].ToDecimal() <= 0)
			{
				strcpy(s.sysmsg, "规格、支数均不能为0");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		else if (tmmsm33["SLAB_TYPE"].ToString() == "5")//模铸
		{

			if (tmmsm33["INGOT_CODE"].ToString().Trim() == "")
			{
				strcpy(s.sysmsg, "锭型不能为空!");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		else
		{

			if (tmmsm33["SLAB_THICK"].ToDecimal() <= 0 || tmmsm33["SLAB_WIDTH"].ToDecimal() <= 0 || tmmsm33["SLAB_LEN"].ToDecimal() <= 0 || tmmsm33["MAT_TUBE"].ToDecimal() <= 0)
			{
				strcpy(s.sysmsg, "规格、支数均不能为0");
				strcpy(s.msg, s.sysmsg);
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}


		cmd_inq.SetCommandText("SELECT CUT_FIN_FLAG FROM TPSSM11 WHERE HEAT_NO = @tmmsm33.HEAT_NO");
		cmd_inq.Parameters.Set("tmmsm33.HEAT_NO", tmmsm33["HEAT_NO"].ToString());
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			if (cmd_inq.GetString(1).Trim() == "1")
			{
				strcpy(s.sysmsg, "该炉次已切割完成");
				strcpy(s.msg, s.sysmsg); //用户提示信息，国际化
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		else
		{
			strcpy(s.sysmsg, "未找到该炉次切割完成标记,可能已炉次确定");
			strcpy(s.msg, s.sysmsg); //用户提示信息，国际化
			throw CApplicationException(-1, s.msg, log.Location);
		}

		tmmsm33["MEASURE_WT_FLAG"] = hmmsm33["MEASURE_WT_FLAG"];

		//if (hmmsm33["SLAB_TYPE"].ToString().Trim() != "3")
		if (hmmsm33["MANAGE_FLAG"].ToString() == "1") //按支管理
		{
			if (tmmsm33["STRAND_NO"].ToString().Trim() == "")
			{
				CFormattable arguments[] = { (const char*)tmmsm01["MAT_NO"].ToString() };// 定义参数列表的数组
				CMessageFormat::Format(s.msg, "材料号[{0}]流号不能为空。", arguments, 1);//格式化字符串
				strcpy(s.sysmsg, s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (hmmsm33["SLAB_THICK"].ToDecimal() != tmmsm01["MAT_THICK"].ToDecimal() || hmmsm33["SLAB_WIDTH"].ToDecimal() != tmmsm01["MAT_WIDTH"].ToDecimal() || hmmsm33["SLAB_LEN"].ToDecimal() != tmmsm01["MAT_LEN"].ToDecimal() || hmmsm33["SLAB_WT"].ToDecimal() != tmmsm01["MAT_ACT_WT"].ToDecimal())
			{
				strcpy(s.msg, "材料主档信息已有变化，不能修改");//格式化字符串
				strcpy(s.sysmsg, s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//若修改了长度且重量未发生变化，则说明可能是短尺，需要根据新长度计算新重量
			if (hmmsm33["SLAB_LEN"].ToDecimal() != tmmsm33["SLAB_LEN"].ToDecimal() && hmmsm33["SLAB_WT"].ToDecimal() > 0)
			{
				tmmsm33["SLAB_WT"] = hmmsm33["SLAB_WT"].ToDecimal() / hmmsm33["SLAB_LEN"].ToDecimal() * tmmsm33["SLAB_LEN"];
				tmmsm33["SLAB_WT"] = tmmsm33["SLAB_WT"].ToDecimal().Round(3);  
			}
			else if (tmmsm01["MEASURE_WT_FLAG"].ToString() == "0")
			{
				doFlag = f_mmsm_get_matwt(bcls_rec, bcls_ret, conn);
				if (ret < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tmmsm33["SLAB_WT"] = bcls_rec->Tables["TMMSM33"].Rows[0]["SLAB_WT"].ToDecimal();
				tmmsm33["MEASURE_WT_FLAG"] = bcls_rec->Tables["TMMSM33"].Rows[0]["MEASURE_WT_FLAG"].ToString();
			}
		}
		else
		{
			if (tmmsm01["SLABTOP_FLAG"].ToString().Trim() == "2")
			{
				strcpy(s.msg, "材料已经出坯结束，不能修改");//格式化字符串
				strcpy(s.sysmsg, s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//Log::Trace("", "", "hmmsm33["MAT_TUBE"] = {0},tmmsm01["MAT_NUM"] = {1}", hmmsm33["MAT_TUBE"].ToDecimal(), tmmsm01["MAT_NUM"].ToDecimal());

			if (hmmsm33["MAT_TUBE"].ToDecimal() != tmmsm01["MAT_NUM_CUT"].ToDecimal())
			{
				strcpy(s.msg, "材料主档支数已有变化，不能修改");//格式化字符串
				strcpy(s.sysmsg, s.msg);
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//Log::Trace("", "", "tmmsm33["MEASURE_WT_FLAG"] = {0},hmmsm33["MEASURE_WT_FLAG"] = {1}", tmmsm33["MEASURE_WT_FLAG"].ToString(), hmmsm33["MEASURE_WT_FLAG"].ToString());
			if (hmmsm33["MAT_TUBE"].ToDecimal() != tmmsm33["MAT_TUBE"].ToDecimal() && hmmsm33["SLAB_WT"].ToDecimal() > 0)
			{
				tmmsm33["SLAB_WT"] = hmmsm33["SLAB_WT"].ToDecimal() / hmmsm33["MAT_TUBE"].ToDecimal() * tmmsm33["MAT_TUBE"];
			}
			else if (tmmsm01["MEASURE_WT_FLAG"].ToString() == "0")
			{
				doFlag = f_mmsm_get_matwt(bcls_rec, bcls_ret, conn);
				if (ret < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				tmmsm33["SLAB_WT"] = bcls_rec->Tables["TMMSM33"].Rows[0]["SLAB_WT"].ToDecimal();
				tmmsm33["MEASURE_WT_FLAG"] = bcls_rec->Tables["TMMSM33"].Rows[0]["MEASURE_WT_FLAG"].ToString();
			}
			

		}


		tmmsm33["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
		tmmsm33["REC_REVISOR"] = s.userid;


		updateColumns = "STRAND_NO,SLAB_CUT_TIME,PROD_SHIFT_GROUP,PROD_SHIFT_NO,SLAB_THICK,SLAB_WIDTH,SLAB_LEN,ADJUST_WIDTH_MARK,SLAB_HEAD_WIDTH,SLAB_TAIL_WIDTH,MEASURE_WT_FLAG,SLAB_WT,MAT_TUBE,IF_TRANSFER,SLAB_PLACE_CODE,REC_REVISOR,REC_REVISE_TIME";

		//Log::Trace("", "", "updateColumns = [{0}]", updateColumns);

		n_count = tmmsm33.Update(updateColumns, "MAT_NO");

		//Log::Trace("", "", "n_count = [{0}]", n_count);

		if (n_count <= 0)
		{
			{
				CFormattable arguments[] = { (const char*)tmmsm33["MAT_NO"].ToString() };// 定义参数列表的数组
				CMessageFormat::Format(s.msg, _RES("MMSMS0000030")/*材料号[{0}炼钢板坯切断实绩信息不存在。*/, arguments, 1);//格式化字符串
				strcpy(s.sysmsg, s.msg);
			}
			throw CApplicationException(-1, s.msg, log.Location);
		}

		tmmsm33.Query();
		
		bcls_rec->Tables["TMMSM33"].Clear();
		tmmsm33.MergeTo(bcls_rec->Tables["TMMSM33"], false);
		bcls_rec->Tables["TMMSM33"].Columns.Add(DT_STRING, "PROC_DIV");
		bcls_rec->Tables["TMMSM33"].Rows[0]["PROC_DIV"] = "U";

		///针对板坯处理炼钢物料主档函数
		doFlag = f_mmsm3301u_proc(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
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
