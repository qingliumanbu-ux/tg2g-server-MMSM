/*******************************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   向萍
Version:    1.0
Date:     2015-07-04
Description: 铸坯产出处理，建物料主档，抛材料申请
***********************************************************************/


/***** C++ 的标准头文件部分 *****/
#include "stdafx.h"


/***** C++ 的业务头文件部分 *****/




int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
//int f_mmsm_get_theorywt(CDecimal MAT_ACT_THICK, CDecimal MAT_ACT_WIDTH, CDecimal MAT_ACT_LEN, CDecimal MAT_NUM, CDecimal &MAT_THEORY_WT);
//获取重量
int f_mmsm_slab_dest(const CString& slab_plan_dest, CString& slab_dest_code, CDbConnection * conn);

#if defined _SYS_MMS || defined _SYS_MES    ////MMS层或MES系统部署时调用的函数
int f_pmom_slab_pct(EIClass *bcls_rec, EIClass *bcls_ret, CDbConnection * conn);   //预定板坯产出更新，只有产出及删除调用。(11)热轧板坯 (20)外卖板坯 (10)厚板板坯
int f_mmsm33_pmof(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);   //炼钢铸坯产出合同跟踪抛帐

#if defined _LINE_HP       //定义有厚板产线
int f_mmhp0009_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif

#endif

#if defined _SYS_PES    //PES层部署时调用的函数
int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);  //调用各模块通讯接口
#endif



BM2_FUNCTION_EXPORT
int f_mmsm3301u_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	//应用处理开始
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int blkNum = 0;
	CString sqlstr = "";
	int pmof_count = 0;
	int mm0099_count = 0;
	int pssm03_count = 0;
	int pmogord_count = 0;
	int slabpct_count = 0;
	int tmmsm33_ic_count = 0;
	int wm00que_count = 0;
	int mmhp0009_count = 0;
	CString   v_slab_dest_code = "";
	CString   v_proc_div = "";    //处理区分：N-新增; U-修改; D-删除; I-累计新增（相同材料分批新增）
	CString   v_order_type = "";
	CString   v_manage_flag = "";

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_sql(conn);

	CModel tmmsm33("TMMSM33");
	CModel tmmsm01("TMMSM01");
	CModel tmmsm96("TMMSM96");


	try
	{
		#pragma region 数据块定义

		/*判断传入参数块是否存在*/
		blkNum = bcls_rec->Tables.IndexOf("TMMSM33");
		if (blkNum < 0)
		{
			strcpy(s.msg, "传入数据块 TMMSM33 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}


		/*物料跟踪履历用，f_mmsm99函数用*/
		blkNum = bcls_rec->Tables.IndexOf("MM0099");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099");
			bcls_rec->Tables["MM0099"].Columns.Add(tmmsm96);
		}

		/*物料跟踪履历用，f_mmsm99 函数INSERT DELETE用*/
		blkNum = bcls_rec->Tables.IndexOf("MM0099_ID");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MM0099_ID");
			bcls_rec->Tables["MM0099_ID"].Columns.Add(tmmsm96);
		}

		/*主档表用，f_mmsm01_icins函数用*/
		blkNum = bcls_rec->Tables.IndexOf("TMMSM33_IC");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("TMMSM33_IC");
			bcls_rec->Tables["TMMSM33_IC"].Columns.Add(tmmsm33);
		}

		/*抛合同跟踪，增加数据参数f_mmsm33_pmof*/
		blkNum = bcls_rec->Tables.IndexOf("PMOF");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("PMOF");
			bcls_rec->Tables["PMOF"].Columns.Add(tmmsm96);
			bcls_rec->Tables["PMOF"].Columns.Add(DT_STRING, "WT");        //重量
			bcls_rec->Tables["PMOF"].Columns.Add(DT_STRING, "PREV_WT");      //前重量
		}

		/*厚板抛合同跟踪，增加数据参数*/
		blkNum = bcls_rec->Tables.IndexOf("MMHP0009");
		//Log::Trace("", __FUNCTION__, "blkNum = bcls_rec->Tables.IndexOf= [{0}]", blkNum);
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMHP0009");
			bcls_rec->Tables["MMHP0009"].Columns.Add(tmmsm96);
		}


		/*置板坯命令产出标志*/
		blkNum = bcls_rec->Tables.IndexOf("PSSM03");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("PSSM03");
			bcls_rec->Tables["PSSM03"].Columns.Add(DT_STRING, "FACTORY_DIV");
			bcls_rec->Tables["PSSM03"].Columns.Add(DT_STRING, "SLAB_NO");       //预定板坯号
			bcls_rec->Tables["PSSM03"].Columns.Add(DT_DECIMAL, "SLAB_NUM");       //预定板坯号
			bcls_rec->Tables["PSSM03"].Columns.Add(DT_STRING, "SLAB_PROD_FLAG");//产出标记	
		}


		/*调用生产预定板坯产出函数用（MMS） f_pmom_slab_pct函数用*/
		blkNum = bcls_rec->Tables.IndexOf("SLABPCT");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("SLABPCT");
			bcls_rec->Tables["SLABPCT"].Columns.Add(DT_STRING, "PREC_SLAB_NO"); //产出板坯
			bcls_rec->Tables["SLABPCT"].Columns.Add(DT_STRING, "PCT_FLAG");   //产出标记 0：产出撤销 1：产出
		}

		/*调用归并合同处理函数用（MMS） f_pmog_order函数用*/
		blkNum = bcls_rec->Tables.IndexOf("PMOGORD");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("PMOGORD");
			bcls_rec->Tables["PMOGORD"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_rec->Tables["PMOGORD"].Columns.Add(DT_DECIMAL, "MAT_WT");
			bcls_rec->Tables["PMOGORD"].Columns.Add(DT_DECIMAL, "MAT_NUM");
			bcls_rec->Tables["PMOGORD"].Columns.Add(DT_STRING, "ORDER_NO");
		}


		#if defined(_WMS_DEPENDENT_SM) 
		/*调用仓库接口（PES）  f_ym00sm_queue函数用*/
		blkNum = bcls_rec->Tables.IndexOf("WM00QUE");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("WM00QUE");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NUM");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "UNIT_CODE");
			bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "OPER_FLAG");

		}
		bcls_rec->Tables["WM00QUE"].Rows.Clear();
		#endif

		blkNum = bcls_rec->Tables.IndexOf("WM00QUE");
		//Log::Trace("", __FUNCTION__, "blkNum = bcls_rec->Tables.WM00QUE= [{0}]", blkNum);
		#pragma endregion

		/* 获取输入参数*/
		bcls_rec->Tables["MM0099_ID"].Rows.Clear();
		bcls_rec->Tables["TMMSM33_IC"].Rows.Clear();
		bcls_rec->Tables["PMOF"].Rows.Clear();
		bcls_rec->Tables["MMHP0009"].Rows.Clear();
		bcls_rec->Tables["PSSM03"].Rows.Clear();
		bcls_rec->Tables["SLABPCT"].Rows.Clear();
		bcls_rec->Tables["PMOGORD"].Rows.Clear();
		
		//Log::Trace("", __FUNCTION__, "begin= [{0}]", bcls_rec->Tables["TMMSM33"].Rows.get_Count());

		
		tmmsm33["SLAB_PLAN_DEST"] = bcls_rec->Tables["TMMSM33"].Rows[0]["SLAB_PLAN_DEST"].ToString();//去向

		//Log::Trace("", "", "tmmsm33["SLAB_PLAN_DEST"] ={0}", tmmsm33["SLAB_PLAN_DEST"].ToString());

		doFlag = f_mmsm_slab_dest(tmmsm33["SLAB_PLAN_DEST"].ToString(), v_slab_dest_code, conn);
		if (doFlag < 0 || v_slab_dest_code.Trim() == "")
		{
			/*sprintf(s.msg, "获取去向出错!");
			throw CApplicationException(-1, s.msg, log.Location);*/
		}
		//Log::Trace("", "", "v_slab_dest_code={0}", v_slab_dest_code);

		bool colExist = bcls_rec->Tables["TMMSM33"].Columns.Contains("PROC_DIV");

        

		for (int i = 0; i < bcls_rec->Tables["TMMSM33"].Rows.get_Count(); i++){
			v_proc_div = colExist ? bcls_rec->Tables["TMMSM33"].Rows[i]["PROC_DIV"].ToString() : "U";
			Log::Trace("", "", "v_proc_div ={0}", v_proc_div);
			tmmsm33.Reset();
			tmmsm33.MergeFrom(bcls_rec->Tables["TMMSM33"].Rows[i]);
			tmmsm33.TrimOrBlank();
			//tmmsm33.Print();

			//Log::Trace("", "", "v_proc_div={0}", v_proc_div);
			//Log::Trace("", "", "tmmsm33["MANAGE_FLAG"] ={0}", tmmsm33["MANAGE_FLAG"].ToString());

			#pragma region 修改处理
			if (v_proc_div == "U")   //U-实绩修改
			{

				#pragma region 更新主档
				//--------读取主档的材料信息------------------
				tmmsm01["MAT_NO"] = tmmsm33["MAT_NO"];
				tmmsm01.Query();

				CDecimal oldMatNum = tmmsm01["MAT_NUM"];
				CDecimal oldMatWt = tmmsm01["MAT_ACT_WT"];

				//--------铸坯主档基本信息修改----------------
				if (tmmsm33["MANAGE_FLAG"].ToString() == "1")//按批管理
				{
					tmmsm01["MAT_NUM"] = tmmsm01["MAT_NUM"].ToDecimal() + (tmmsm33["MAT_TUBE"].ToDecimal() - tmmsm01["MAT_NUM_CUT"].ToDecimal());
					tmmsm01["MAT_TUBE"] = tmmsm01["MAT_NUM"];
					tmmsm01["MAT_NUM_CUT"] = tmmsm33["MAT_TUBE"];
					//tmmsm01["MAT_ACT_WT"] = tmmsm01["MAT_NUM"].ToDecimal() * (tmmsm33["SLAB_WT"].ToDecimal() / tmmsm33["MAT_TUBE"].ToDecimal());
				}
				else
				{
					//tmmsm01["MAT_ACT_WT"] = tmmsm33["SLAB_WT"];
				}

				//doFlag = f_mmsm_get_theorywt(tmmsm33["SLAB_THICK"].ToDecimal(), tmmsm33["SLAB_WIDTH"].ToDecimal(), tmmsm33["SLAB_LEN"].ToDecimal(), tmmsm01["MAT_NUM"].ToDecimal(), tmmsm01["MAT_THEORY_WT"].ToDecimal());
				//if (doFlag < 0)
				//{
				//	throw CApplicationException(-1, s.msg, log.Location);
				//}
				//Log::Trace("", "", "理论重量tmmsm01["MAT_THEORY_WT"] = {0}", tmmsm01["MAT_THEORY_WT"].ToDecimal());
				//Log::Trace("", "", "tmmsm33["SLAB_HEAD_WIDTH"] = {0}", tmmsm33["SLAB_HEAD_WIDTH"].ToDecimal());
				//Log::Trace("", "", "tmmsm33["SLAB_TAIL_WIDTH"] = {0}", tmmsm33["SLAB_TAIL_WIDTH"].ToDecimal());

				tmmsm01["MAT_ACT_THICK"] = tmmsm33["SLAB_THICK"];
				tmmsm01["MAT_ACT_WIDTH"] = tmmsm33["SLAB_WIDTH"];
				tmmsm01["MAT_ACT_LEN"] = tmmsm33["SLAB_LEN"];
				tmmsm01["MAT_THICK"] = tmmsm33["SLAB_THICK"];
				tmmsm01["MAT_WIDTH"] = tmmsm33["SLAB_WIDTH"];
				tmmsm01["MAT_LEN"] = tmmsm33["SLAB_LEN"];
				tmmsm01["L2_THEORY_WT"] = tmmsm33["L2_THEORY_WT"];
				if (tmmsm01["RECEIVE_WEIGHT"].ToDecimal() == 0 || tmmsm01["RECEIVE_WEIGHT"].ToDecimal() < 0 || tmmsm01["RCV_MAT_FLAG"].ToString() =="N")
				{
					//有称重量取称重量，没有称重量取理论量
					if (tmmsm01["MEASURE_WT"].ToDecimal() != 0)
					{
						tmmsm01["MAT_WT"] = tmmsm01["MEASURE_WT"];
					}
					
					//材料二级理论重量   此处更改为不修改，会影响盘库的修改 以及收货后数据
					//20240425  mfj 更新  当收货重量为0时才更新，否则不更新MAT_WT
				}
				else
				{
					tmmsm01["MAT_WT"] = tmmsm01["MAT_ACT_WT"];
				}
				//tmmsm01["MAT_WT"] = tmmsm33["SLAB_WT"];
				tmmsm01["SLAB_HEAD_WIDTH"] = tmmsm33["SLAB_HEAD_WIDTH"];
				tmmsm01["SLAB_TAIL_WIDTH"] = tmmsm33["SLAB_TAIL_WIDTH"];
				tmmsm01["SLAB_TAPPER_WIDTH_START"] = tmmsm33["SLAB_TAPPER_WIDTH_START"];
				tmmsm01["SLAB_TAPPER_WIDTH_LEN"] = tmmsm33["SLAB_TAPPER_WIDTH_LEN"];
				tmmsm01["REC_REVISE_TIME"] = tmmsm33["REC_REVISE_TIME"];
				tmmsm01["REC_REVISOR"] = tmmsm33["REC_REVISOR"];
				tmmsm01["PROD_TIME"] = tmmsm33["SLAB_CUT_TIME"];
				tmmsm01["PROD_SHIFT_NO"] = tmmsm33["PROD_SHIFT_NO"];
				tmmsm01["PROD_SHIFT_GROUP"] = tmmsm33["PROD_SHIFT_GROUP"];
				tmmsm01["MAT_NO"] = tmmsm33["MAT_NO"];
				tmmsm01["IF_TRANSFER"] = tmmsm33["IF_TRANSFER"];
				tmmsm01["SLAB_PLACE_CODE"] = tmmsm33["SLAB_PLACE_CODE"];
				tmmsm01["MEASURE_WT_FLAG"] = tmmsm33["MEASURE_WT_FLAG"];
				tmmsm01["REC_REVISE_TIME"] = CDateTime::Now().ToString("yyyyMMddHHmmss");
				tmmsm01["REC_REVISOR"] = s.userid;
				tmmsm01["SLAB_NO"] = tmmsm33["SLAB_NO"];//板坯号
				tmmsm01["PRINT_NO"] = tmmsm33["PRINT_NO"];//喷印号
				tmmsm01["SM_PLAN_NO"] = tmmsm33["SM_PLAN_NO"];//三级计划号
				tmmsm01["SM_PLAN_NOL2"] = tmmsm33["SM_PLAN_NOL2"];//二级计划号

				//抛物料履历跟踪
				bcls_rec->Tables["MM0099"].Rows.Clear();
				bcls_rec->Tables["MM0099"].Rows.Add();
				bcls_rec->Tables["MM0099"].Rows[0].Merge(tmmsm01);
				if (tmmsm01["IC_CC_FLAG"].ToString().Trim() == "I")
				{
					bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM60";
				}
				else
				{
					bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM10";
				}
				
				bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "SM";
				bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
				bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = "f_mmsm3301u_proc";

				doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
				if (doFlag < 0)
				{
					throw CApplicationException(-1, s.msg, log.Location);
				}
				#pragma endregion

				#pragma region MMS层生产模块抛合同跟踪

				//--------铸坯产出修改：抛合同跟踪----------------
				//单记录
				////Log::Info("", __FUNCTION__, "修改：抛合同跟踪 ORDER_NO=[{0}], PONO_SLAB=[{1}]", tmmsm01["ORDER_NO"].ToString(), tmmsm33["PONO_SLAB_1"].ToString());

				if (tmmsm01["ORDER_NO"].ToString().Trim() != "")
				{
					if (v_slab_dest_code == "HP")
					{
						#if defined _LINE_HP       //定义有厚板产线
						//Log::Trace("", __FUNCTION__, "调用 f_mmhp0009_proc 739行 tmmsm01["MAT_NO"] = [{0}]", tmmsm01["MAT_NO"].ToString());
						bcls_rec->Tables["MMHP0009"].Rows.Add();
						bcls_rec->Tables["MMHP0009"].Rows[mmhp0009_count].Merge(tmmsm01);
						bcls_rec->Tables["MMHP0009"].Rows[mmhp0009_count]["EVENT_ID"] = "54";

						mmhp0009_count++;
						#endif
					}
					else
					{
						if (tmmsm33["MANAGE_FLAG"].ToString() == "1")//按批管理
						{
							bcls_rec->Tables["PMOF"].Rows.Add();
							bcls_rec->Tables["PMOF"].Rows[pmof_count].Merge(tmmsm01);
							bcls_rec->Tables["PMOF"].Rows[pmof_count]["EVENT_ID"] = "5QSM";  //5Q-产出确定
							bcls_rec->Tables["PMOF"].Rows[pmof_count]["WT"] = tmmsm33["SLAB_WT"].ToDecimal() - oldMatWt;
							pmof_count++;
						}
						else
						{
							bcls_rec->Tables["PMOF"].Rows.Add();
							bcls_rec->Tables["PMOF"].Rows[pmof_count].Merge(tmmsm01);
							bcls_rec->Tables["PMOF"].Rows[pmof_count]["EVENT_ID"] = "54";  //54-材料重量修改
							bcls_rec->Tables["PMOF"].Rows[pmof_count]["PREV_WT"] = oldMatWt;
							pmof_count++;
						}
					}
				}
				#pragma endregion

			}
			#pragma endregion

			#pragma region 向外部系统发送切割实绩
			#if defined(_SYS_PES)
			//blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
			//if (blkNum < 0)
			//{
			//	bcls_rec->Tables.Add("MMSMSND");
			//	bcls_rec->Tables["MMSMSND"].Clone(bcls_rec->Tables["TMMSM33"]);
			//	bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TC_BACKLOG");
			//}
			//bcls_rec->Tables["MMSMSND"].Rows.Clear();
			//bcls_rec->Tables["MMSMSND"].Rows.Add();
			////Log::Trace("", __FUNCTION__, "调用 向外部系统发送切割实绩 [{0}],[{1}],[{2}]", bcls_rec->Tables["MMSMSND"].Rows.get_Count(),bcls_rec->Tables["TMMSM33"].Rows.get_Count(),i);

			//bcls_rec->Tables["MMSMSND"].Rows[0].Merge(bcls_rec->Tables["TMMSM33"].Rows[i]);
			//
			//if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("TC_BACKLOG"))
			//{
			//	bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TC_BACKLOG");
			//}
	
			//bcls_rec->Tables["MMSMSND"].Rows[0]["TC_BACKLOG"] = "MMSM33";
			//doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);
			//if (doFlag < 0)
			//{
			//	throw CApplicationException(-1, s.msg, log.Location);
			//}
			#endif  //_SYS_PES
			#pragma endregion
			
		}

   
		#pragma region 调用物料跟踪
		//Log::Trace("", "", " MM0099_ID_count {0}", bcls_rec->Tables["MM0099_ID"].Rows.get_Count());
		if (bcls_rec->Tables["MM0099_ID"].Rows.get_Count() > 0)
		{
			bcls_rec->Tables["MM0099"].Rows.Clear();
			//Log::Trace("", "", " MM0099_count 0 {0}", bcls_rec->Tables["MM0099"].Rows.get_Count());
			for (int i = 0; i < bcls_rec->Tables["MM0099_ID"].Rows.get_Count(); i++){
				bcls_rec->Tables["MM0099"].Rows.Add();
				bcls_rec->Tables["MM0099"].Rows[i].Merge(bcls_rec->Tables["MM0099_ID"].Rows[i]);
				//Log::Trace("", "", " i [{0}]", i);
			}
			//Log::Trace("", "", " MM0099_count 1 {0}", bcls_rec->Tables["MM0099"].Rows.get_Count());
			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		#pragma endregion

        #pragma region 调用生产接口
        #if defined _SYS_MMS || defined _SYS_MES 
		
		#pragma region 非厚板合同跟踪抛帐
		if (bcls_rec->Tables["PMOF"].Rows.get_Count() > 0)
		{
			doFlag = f_mmsm33_pmof(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		#pragma endregion

		#pragma region 厚板向合同跟踪
		#if defined _LINE_HP       //定义有厚板产线
		if (bcls_rec->Tables["MMHP0009"].Rows.get_Count() > 0)
		{
			doFlag = f_mmhp0009_proc(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
		}
		#endif
		#pragma endregion

        #endif
        #pragma endregion

		//调用仓库函数 自动出入库或生成入库队列

	#pragma endregion
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}


	return doFlag;

}
