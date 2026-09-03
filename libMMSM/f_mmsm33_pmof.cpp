/* ****************************************************************************
*	Copyright (c) Baosight Corporation 2011 . All Rights Reserved.
*  	BM2PES 宝信生产执行系统
*****************************************************************************
*  程序名称			: f_mmsm33_pmof
*  程序描述			: 炼钢铸坯产出向合同跟踪抛帐
*  备注说明			:
*  修改历史			:
*       2016-6-4   向萍
*			... ...
* **************************************************************************** */
#include "stdafx.h" 

 /*炼钢物料主表*/
 /*炼钢物料主表*/  


#if defined _SYS_MMS || defined _SYS_MES 

#endif


BM2_FUNCTION_IMPORT

#if defined _SYS_MMS || defined _SYS_MES   
int f_pmof99_v3(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif

// 函数入口
BM2_FUNCTION_EXPORT
int f_mmsm33_pmof(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	int doFlag = 0;
	int ret = 0;
	int	rowNum = 0;
	int n_count = 0;
	CDecimal v_prev_wt = 0;
	CString v_mat_id = "";

	EIClass inBlock;          //抛合同跟踪用数据块

	CString sqlstr = "";
	CModel tmmsm01("TMMSM01");
	CModel hmmsm01("HMMSM01");

    #if defined _SYS_MMS || defined _SYS_MES  
	CModel tpmof03("TPMOF03");
    #endif

	CDbCommand cmd_sql(conn);
	CDbCommand cmd_inq(conn);

	try
	{
		#if defined _SYS_MMS || defined _SYS_MES 

		//----------------------------------------------
		//定义函数调用传入数据块
		//合同跟踪
		if (inBlock.Tables.Contains("PMOF99") == false)
		{
			inBlock.Tables.Add("PMOF99");
			inBlock.Tables["PMOF99"].Columns.Add(tpmof03);
		}


		//----------------------------------------------
		//判断是否存在传入参数数据块，使用[PMOF]块
		if (bcls_rec->Tables.Contains("PMOF") == false)
		{
			strcpy(s.msg, _RES("GCRSS0000011")/*系统出现异常，数据块有误，请联系系统维护人员。*/);
			strcpy(s.sysmsg, "抛合同跟踪处理函数输入参数块[PMOF]不存在。");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		/*获得传入参数*/
		rowNum = bcls_rec->Tables["PMOF"].Rows.get_Count();

		inBlock.Tables["PMOF99"].Rows.Clear();
		for (int i = 0; i < rowNum; i++)
		{
			/*tpmof03.Reset();
			tpmof03.MergeFrom(bcls_rec->Tables["PMOF"].Rows[i]);
			tmmsm01["MAT_NO"] = bcls_rec->Tables["PMOF"].Rows[i]["MAT_NO"].ToString().Trim();*/

			tmmsm01.Reset();
			//获取传入参数
			tmmsm01.MergeFrom(bcls_rec->Tables["PMOF"].Rows[i]);

			tpmof03["EVENT_ID"] = bcls_rec->Tables["PMOF"].Rows[i]["EVENT_ID"].ToString().Trim();
			tpmof03["WT"] = bcls_rec->Tables["PMOF"].Rows[i]["WT"].ToDecimal();
			tpmof03["PREV_WT"] = bcls_rec->Tables["PMOF"].Rows[i]["PREV_WT"].ToDecimal();

			
			////Log::Info("", __FUNCTION__, "传入参数, EVENT_ID  = [{0}]", tpmof03["EVENT_ID"].ToString());			//52-产出新增; 53-材料产出撤销; 54-材料重量修改
			////Log::Info("", __FUNCTION__, "传入参数, MAT_NO    = [{0}]", tmmsm01["MAT_NO"].ToString());
			////Log::Info("", __FUNCTION__, "传入参数, MAT_WT    = [{0}]", tpmof03["WT"].ToDecimal());
			////Log::Info("", __FUNCTION__, "传入参数, PREV_WT    = [{0}]", tpmof03["PREV_WT"].ToDecimal());


			////查询材料主档信息
			//sqlstr = "tmmsm01.Query(MAT_NO)";
			//tmmsm01.Query("MAT_NO");

			
			//Log::Trace("", __FUNCTION__, "读取材料tmmsm01["MAT_NO"] =[{0}]主档, WHOLE_BACKLOG=[{1}]", tmmsm01["MAT_NO"].ToString(), tmmsm01["WHOLE_BACKLOG"].ToString());


			// ADD BY XP 2016/05/30  取PONO_SLAB对应的计划重量
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	    // Oracle 数据库
			default:
				sqlstr = "SELECT slab_wt "
					"  FROM TPSSM03 "
					" WHERE SLAB_NO	= @slab_no";
				break;
			}
			cmd_sql.SetCommandText(sqlstr);
			cmd_sql.Parameters.Clear();
			cmd_sql.Parameters.Set("slab_no", tmmsm01["PONO_SLAB"].ToString());
			cmd_sql.ExecuteReader();
			if (cmd_sql.Read())
			{
				v_prev_wt = cmd_sql.GetDecimal(1);

			}
			cmd_sql.Close();

			//---------------------------------------
			//赋值
			//Log::Trace("", __FUNCTION__, "读取材料tmmsm01["ORDER_NO"] =[{0}]主档, WHOLE_BACKLOG=[{1}]", tmmsm01["ORDER_NO"].ToString(), tmmsm01["WHOLE_BACKLOG"].ToString());

			tpmof03["SYSTEM_ID"] = "MMSM";
			tpmof03["FUNC_ID"] = CString(s.svc_name);
			tpmof03["MAT_NO"] = tmmsm01["MAT_NO"];
			tpmof03["ORDER_NO"] = tmmsm01["ORDER_NO"];

			tpmof03["WHOLE_BACKLOG_NO"] = tmmsm01["WHOLE_BACKLOG_NO"];      // 全程工序途径码顺序号
			tpmof03["WHOLE_BACKLOG"] = tmmsm01["WHOLE_BACKLOG"];         // 全程工序途径码


			if (tpmof03["EVENT_ID"].ToString() == "52SM" || tpmof03["EVENT_ID"].ToString() == "52TC") //52-产出新增;
			{
				tpmof03["MAT_STATUS"] = tmmsm01["MAT_STATUS"];
				tpmof03["WHOLE_BACKLOG_SEQ"] = tmmsm01["WHOLE_BACKLOG_SEQ"];		// 全程工序顺序号
				tpmof03["WHOLE_BACKLOG_CODE"] = tmmsm01["WHOLE_BACKLOG_CODE"];	// 全程工序代码
				tpmof03["WT"] = tmmsm01["MAT_ACT_WT"];
				tpmof03["NUM"] = 1;
				tpmof03["PREV_MAT_NO"] = tmmsm01["PONO_SLAB"];
				tpmof03["PREV_WT"] = v_prev_wt;
				tpmof03["PREV_NUM"] = 1;

			}

			if (tpmof03["EVENT_ID"].ToString() == "53SM" || tpmof03["EVENT_ID"].ToString() == "53TC") //53-材料产出撤销;
			{
				//tpmof03["MAT_STATUS"] = tmmsm01["MAT_STATUS"];
				tpmof03["MAT_STATUS"] = "23";
				tpmof03["WHOLE_BACKLOG_SEQ"] = tmmsm01["WHOLE_BACKLOG_SEQ"];		// 全程工序顺序号
				tpmof03["WHOLE_BACKLOG_CODE"] = tmmsm01["WHOLE_BACKLOG_CODE"];	// 全程工序代码
				tpmof03["WT"] = tmmsm01["MAT_ACT_WT"];
				tpmof03["NUM"] = 1;
				tpmof03["PREV_MAT_NO"] = tmmsm01["PONO_SLAB"];
				tpmof03["PREV_WT"] = v_prev_wt;
				tpmof03["PREV_NUM"] = 1;

			}

			if (tpmof03["EVENT_ID"].ToString() == "54") //54-材料重量修改;
			{

				tpmof03["WT"] = tmmsm01["MAT_ACT_WT"];

				tpmof03["WHOLE_BACKLOG_SEQ"] = tmmsm01["NEXT_WHOLE_BACKLOG_SEQ"];   //后全程工序顺序号
				tpmof03["WHOLE_BACKLOG_CODE"] = tmmsm01["NEXT_WHOLE_BACKLOG_CODE"];  //后全程工序代码

				tpmof03["MAT_STATUS"] = tmmsm01["MAT_STATUS"];
				tpmof03["PREV_MAT_STATUS"] = tmmsm01["MAT_STATUS"];

				if (tmmsm01["PLAN_NO"].ToString().Trim() != "" || tmmsm01["TRANSFER_FLAG"].ToString().Trim() != "0")
				{
					tpmof03["IF_PLAN"] = "1";
				}
				else
				{
					tpmof03["IF_PLAN"] = "0";
				}

			}

			if (tpmof03["EVENT_ID"].ToString() == "59") //59-解封锁;
			{
				tpmof03["WHOLE_BACKLOG_CODE"] = tmmsm01["NEXT_WHOLE_BACKLOG_CODE"];
				tpmof03["WHOLE_BACKLOG_SEQ"] = tmmsm01["NEXT_WHOLE_BACKLOG_SEQ"];
				tpmof03["WT"] = tmmsm01["MAT_ACT_WT"];
				tpmof03["NUM"] = 1;

				tpmof03["MAT_STATUS"] = "23";
				tpmof03["PREV_MAT_STATUS"] = "21";
			}

			if (tpmof03["EVENT_ID"].ToString() == "5QSM") //5Q-产出确定;
			{
				tpmof03["WHOLE_BACKLOG_SEQ"] = tmmsm01["WHOLE_BACKLOG_SEQ"];		// 全程工序顺序号
				tpmof03["WHOLE_BACKLOG_CODE"] = tmmsm01["WHOLE_BACKLOG_CODE"];	// 全程工序代码
			}

			//Log::Trace("", __FUNCTION__, "读取材料tmmsm01["MAT_NO"] =[{0}]主档, MAT_STATUS=[{1}]", tmmsm01["MAT_NO"].ToString(), tpmof03["MAT_STATUS"].ToString());

			inBlock.Tables["PMOF99"].Rows.Add();
			//单记录调用
			inBlock.Tables["PMOF99"].Rows[i].Merge(tpmof03);

		}

		ret = f_pmof99_v3(&inBlock, bcls_ret, conn);
		if (ret < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}
       #endif

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
