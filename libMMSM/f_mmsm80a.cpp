/*<remark>=========================================================
/// <summary>
/// 方坯物料信息组批
/// <para>
/// * 调用物料组批函数
/// <para>
/// </summary>
/// <returns></returns>
===========================================================</remark>*/
#include "stdafx.h"

 
 

#if defined _SYS_MMS || defined _SYS_MES    //MMS层或MES系统部署时调用的函数

#endif


BM2_FUNCTION_EXPORT
int f_mmsm99(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

//int f_wm00_queue(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#if defined _SYS_MMS || defined _SYS_MES    //MMS层或MES系统部署时调用的函数
int f_pmof99_v3(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif
#if  defined _SYS_PES
//int f_mmsm009a_snd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);
#endif

int f_mmsm80a(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);//系统日志类定义

	int doFlag = 0;
	CString sqlstr = " ";
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CString	cs_main_mat_flag("");
	int j = 0;
	CString resumeSeqNo = "";
	CString tcBacklog = "";
	CString ymque = "";
	CString funcId = "";
	CString eventDesc = "";
	CString unPlanNo = "";
	CString unOrderNo = "";
	CString ifTrace = "";
	CDecimal matTube = 0;
	CDecimal matActWt = 0;
	CDecimal matTheoryWt = 0;
	CString ifTcSnd = "";
	CDecimal v_main_mat_wt = 0;
	CString PROD_SHIFT_NO = "";
	CString PROD_SHIFT_GROUP = "";
	CString aimMatNo = "";
	CString aimHeatNo = "";
	CString aimPono = "";

	/* 实体类定义 */
	CModel tmmsm01("TMMSM01");
	CModel tmmsm01_main("TMMSM01");
	CModel tmmsm35("TMMSM35");

#if defined _SYS_MMS || defined _SYS_MES    //MMS层或MES系统部署时调用的函数
	CModel tpmof03("TPMOF03");
#endif

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		CDbCommand getSeq("SELECT MMSM_MATNO_SEQ.NEXTVAL  FROM DUAL", conn);
		CString newSeqNo = "0000" + getSeq.ExecuteScalar().ToString().Trim();
		newSeqNo = newSeqNo.Substring(newSeqNo.GetLength() - 4);
		//Log::Trace("", "", "newSeqNo=[{0}]", newSeqNo);
		resumeSeqNo = datetime.Trim() + newSeqNo;
		//Log::Trace("", "", "resumeSeqNo=[{0}]", resumeSeqNo);

		tmmsm35["REC_CREATE_TIME"] = datetime;
		tmmsm35["REC_CREATOR"] = s.userid;
		tmmsm35["PROD_TIME"] = datetime;

		/*--------------------------------------------------------------班次班组计算---------------------------------------------------------------------*/
#if defined _SYS_PES || defined _SYS_MES
		//Log::Trace("", "", "tmmsm35["PROD_TIME"] =[{0}]", tmmsm35["PROD_TIME"].ToString());
		//f_epep_get_shift_group("SM", tmmsm35["PROD_TIME"].ToString(), tmmsm35["PROD_SHIFT_NO"].ToString(), tmmsm35["PROD_SHIFT_GROUP"].ToString(), conn);
		//Log::Trace("", "", "tmmsm35["PROD_SHIFT_NO"] =[{0}]", tmmsm35["PROD_SHIFT_NO"].ToString());
		//Log::Trace("", "", "tmmsm35["PROD_SHIFT_GROUP"] =[{0}]", tmmsm35["PROD_SHIFT_GROUP"].ToString());
		f_epep_get_shift_group("SM", tmmsm35["PROD_TIME"].ToString(), PROD_SHIFT_NO, PROD_SHIFT_GROUP, conn);
		//Log::Trace("", "", "tmmsm35["PROD_SHIFT_NO"] =[{0}]", tmmsm35["PROD_SHIFT_NO"].ToString());
		//Log::Trace("", "", "tmmsm35["PROD_SHIFT_GROUP"] =[{0}]", tmmsm35["PROD_SHIFT_GROUP"].ToString());
		tmmsm35["PROD_SHIFT_NO"] = PROD_SHIFT_NO;
		tmmsm35["PROD_SHIFT_GROUP"] = PROD_SHIFT_GROUP;
		/*-----------------------------------------------------------------------------------------------------------------------------------------------*/
#endif

		tmmsm35["PROD_MAKER"] = s.userid;
		tmmsm35["PROD_SEQ_NO"] = resumeSeqNo;

		blkNum = bcls_rec->Tables.IndexOf("MMSM80");
		if (blkNum < 0)
		{
			strcpy(s.msg, _RES("GCRSS0000007")/*系统出现异常，请联系系统维护人员。*/);
			strcpy(s.sysmsg, "传入数据块 MMSM80 不存在。");
			throw CApplicationException(-1, s.msg, s.svc_name);
		}


		blkNum = bcls_rec->Tables.IndexOf("PMOF99");
		if (blkNum <= 0)
		{
			bcls_rec->Tables.Add("PMOF99");
		}


		/* 添加并设置块名 */
		if (!bcls_rec->Tables.Contains("MM0099"))
		{
			bcls_rec->Tables.Add("MM0099");
		}
		if (!bcls_rec->Tables["MM0099"].Columns.Contains("EVENT_ID"))
		{
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_ID");
		}
		if (!bcls_rec->Tables["MM0099"].Columns.Contains("EVENT_LINE_TYPE"))
		{
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_LINE_TYPE");
		}
		if (!bcls_rec->Tables["MM0099"].Columns.Contains("SYSTEM_ID"))
		{
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "SYSTEM_ID");
		}
		if (!bcls_rec->Tables["MM0099"].Columns.Contains("FUNC_ID"))
		{
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "FUNC_ID");
		}
		if (!bcls_rec->Tables["MM0099"].Columns.Contains("EVENT_DESC"))
		{
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "EVENT_DESC");
		}
		if (!bcls_rec->Tables["MM0099"].Columns.Contains("MAT_NO"))
		{
			bcls_rec->Tables["MM0099"].Columns.Add(DT_STRING, "MAT_NO");
		}
		if (bcls_rec->Tables["MM0099"].Rows.get_Count() <= 0)
		{
			bcls_rec->Tables["MM0099"].Rows.Add();
		}

		blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("MMSMSND");
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TABLE_TYPE");
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "MAIN_MAT_NO");
			bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "MAT_NO");
		}

		if (bcls_rec->Tables["MMSM80"].Columns.Contains("TC_BACKLOG"))	//炼钢侧物料管理母材料拆批
		{
			tcBacklog = bcls_rec->Tables["MMSM80"].Rows[0]["TC_BACKLOG"].ToString().Trim();
		}
		if (bcls_rec->Tables["MMSM80"].Columns.Contains("YMQUE"))                       //用于判断拆出的子坯是否需要建立入库队列
		{
			ymque = bcls_rec->Tables["MMSM80"].Rows[0]["YMQUE"].ToString().Trim();
		}
		if (bcls_rec->Tables["MMSM80"].Columns.Contains("FUNC_ID"))
		{
			funcId = bcls_rec->Tables["MMSM80"].Rows[0]["FUNC_ID"].ToString().Trim();
		}
		if (bcls_rec->Tables["MMSM80"].Columns.Contains("EVENT_DESC"))
		{
			eventDesc = bcls_rec->Tables["MMSM80"].Rows[0]["EVENT_DESC"].ToString().Trim();
		}
		if (bcls_rec->Tables["MMSM80"].Columns.Contains("UNPLAN"))
		{
			unPlanNo = bcls_rec->Tables["MMSM80"].Rows[0]["UNPLAN"].ToString().Trim();
		}
		if (bcls_rec->Tables["MMSM80"].Columns.Contains("UNORDER"))
		{
			unOrderNo = bcls_rec->Tables["MMSM80"].Rows[0]["UNORDER"].ToString().Trim();
		}
		if (bcls_rec->Tables["MMSM80"].Columns.Contains("IF_TRACE"))
		{
			ifTrace = bcls_rec->Tables["MMSM80"].Rows[0]["IF_TRACE"].ToString().Trim();
		}

		ifTcSnd = "1";//默认需要调用发送电文（考虑其他模块调用）
		if (bcls_rec->Tables["MMSM80"].Columns.Contains("IF_TC_SND"))
		{
			ifTcSnd = bcls_rec->Tables["MMSM80"].Rows[0]["IF_TC_SND"].ToString().Trim();
		}

		#pragma region  获取母材料信息
		for (int i = 0; i < bcls_rec->Tables["MMSM80"].Rows.get_Count(); i++)
		{
			cs_main_mat_flag = bcls_rec->Tables["MMSM80"].Rows[i]["MAIN_MAT_FLAG"].ToString().Trim();
			//Log::Trace("", __FUNCTION__, "cs_main_mat_flag		= [{0}]", (const char*)cs_main_mat_flag);

			if (cs_main_mat_flag != "1") continue;

			/* 判断主材料 */
			tmmsm01_main["MAT_NO"] = bcls_rec->Tables["MMSM80"].Rows[i]["MAT_NO"].ToString().Trim();	//主材料号
			aimMatNo = bcls_rec->Tables["MMSM80"].Rows[i]["AIM_MAT_NO"].ToString().Trim();	//组批材料号
			aimHeatNo = bcls_rec->Tables["MMSM80"].Rows[i]["AIM_HEAT_NO"].ToString().Trim();	//组批熔炼号
			aimPono = bcls_rec->Tables["MMSM80"].Rows[i]["AIM_PONO"].ToString().Trim();	//组批制造命令号
			//Log::Trace("", __FUNCTION__, "tmmsm01_main.MAT_NO		= [{0}]", tmmsm01_main["MAT_NO"].ToString());
			//Log::Trace("", __FUNCTION__, "aimMatNo		= [{0}]", aimMatNo);
			//Log::Trace("", __FUNCTION__, "aimHeatNo		= [{0}]", aimHeatNo);
			//Log::Trace("", __FUNCTION__, "aimPono		= [{0}]", aimPono);

			if (!tmmsm01_main.Query("MAT_NO"))
			{
				sprintf(s.msg, "母材料不存在，不能并批!");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			//取母材料的原重量
			v_main_mat_wt = tmmsm01_main["MAT_ACT_WT"];

			if (tmmsm01_main["MAT_SHAPE_FLAG"].ToString().Trim() != "2")
			{
				sprintf(s.msg, "请选择形态符合要求的材料进行并批!");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (unPlanNo == "1" && tmmsm01["PLAN_NO"].ToString().Trim() != "")
			{
				sprintf(s.msg, "材料有计划，不允许在此并批");
				strcpy(s.sysmsg, "材料有计划，不允许在此并批");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			if (unOrderNo == "1" && tmmsm01["ORDER_NO"].ToString().Trim() != "")
			{
				sprintf(s.msg, "材料有合同，不允许在此并批");
				strcpy(s.sysmsg, "材料有合同，不允许在此并批");
				throw CApplicationException(-1, s.msg, log.Location);
			}


			/* 跳出循环 */
			break;
		}
		#pragma endregion

		if (tmmsm01_main["MAT_NO"].ToString().Trim() == "")
		{
			sprintf(s.msg, "请先选择母材料进行并批!");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		#pragma region  处理子材料信息
		for (int i = 0; i < bcls_rec->Tables["MMSM80"].Rows.get_Count(); i++)
		{
			/* 获取被并批材料 */
			tmmsm01["MAT_NO"] = bcls_rec->Tables["MMSM80"].Rows[i]["MAT_NO"].ToString().Trim();	//被并批材料号
			//Log::Trace("", __FUNCTION__, "tmmsm01.MAT_NO		= [{0}]", (const char*)tmmsm01["MAT_NO"].ToString());

			if (!tmmsm01.Query("MAT_NO"))
			{
				sprintf(s.msg, "未找到子材料信息!");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			/* 校验能否并批 */
			if (tmmsm01["MAT_LINE_TYPE"].ToString().Trim() != tmmsm01_main["MAT_LINE_TYPE"].ToString().Trim())
			{
				sprintf(s.msg, "请选择形态符合要求的材料进行并批!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm01["MAT_SHAPE_FLAG"].ToString().Trim() != tmmsm01_main["MAT_SHAPE_FLAG"].ToString().Trim())
			{
				sprintf(s.msg, "请选择形态符合要求的材料进行并批!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm01["PLAN_NO"].ToString().Trim() != tmmsm01_main["PLAN_NO"].ToString().Trim())
			{
				sprintf(s.msg, "计划号不同不能进行并批!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm01["ORDER_NO"].ToString().Trim() != tmmsm01_main["ORDER_NO"].ToString().Trim())
			{
				sprintf(s.msg, "合同号不同不能进行并批!");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm01["STOCK_NO"].ToString().Trim() != tmmsm01_main["STOCK_NO"].ToString().Trim())
			{
				sprintf(s.msg, "库区不同,不能并批。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm01["IN_FLAG"].ToString().Trim() != tmmsm01_main["IN_FLAG"].ToString().Trim())
			{
				sprintf(s.msg, "入库标记不同,不能并批。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm01["SURFACE_DECIDE_CODE"].ToString().Trim() != tmmsm01_main["SURFACE_DECIDE_CODE"].ToString().Trim())
			{
				sprintf(s.msg, "表面判定代码不同,不能并批。");
				throw CApplicationException(-1, s.msg, log.Location);
			}
			if (tmmsm01["COMPLEX_DECIDE_CODE"].ToString().Trim() != tmmsm01_main["COMPLEX_DECIDE_CODE"].ToString().Trim())
			{
				sprintf(s.msg, "综合判定代码不同,不能并批。");
				throw CApplicationException(-1, s.msg, log.Location);
			}

			/* 设置母材料的支数和重量 */


			matTube = matTube + tmmsm01["MAT_NUM"].ToDecimal();
			matActWt = matActWt + tmmsm01["MAT_ACT_WT"].ToDecimal();
			matTheoryWt = matTheoryWt + tmmsm01["MAT_THEORY_WT"].ToDecimal();

			if (ifTrace == "1")
			{
				tmmsm35["IN_MAT_NO"] = tmmsm01["MAT_NO"];
				tmmsm35["IN_HEAT_NO"] = tmmsm01["HEAT_NO"];
				tmmsm35["IN_PONO"] = tmmsm01["PONO"];
				tmmsm35["IN_SG_SIGN"] = tmmsm01["SG_SIGN"];
				tmmsm35["IN_ST_NO"] = tmmsm01["ST_NO"];
				tmmsm35["IN_MAT_THICK"] = tmmsm01["MAT_THICK"];
				tmmsm35["IN_MAT_WIDTH"] = tmmsm01["MAT_WIDTH"];
				tmmsm35["IN_MAT_LEN"] = tmmsm01["MAT_LEN"];
				tmmsm35["IN_MAT_WT"] = tmmsm01["MAT_ACT_WT"];
				tmmsm35["IN_MAT_TUBE"] = tmmsm01["MAT_NUM"];
				tmmsm35["MAT_NO"] = aimMatNo;
				tmmsm35["HEAT_NO"] = aimHeatNo;
				tmmsm35["PONO"] = aimPono;
				tmmsm35["SG_SIGN"] = tmmsm01_main["SG_SIGN"];
				tmmsm35["ST_NO"] = tmmsm01_main["ST_NO"];
				tmmsm35["MAT_THICK"] = tmmsm01_main["MAT_THICK"];
				tmmsm35["MAT_WIDTH"] = tmmsm01_main["MAT_WIDTH"];
				tmmsm35["MAT_LEN"] = tmmsm01_main["MAT_LEN"];
				tmmsm35["MAT_WT"] = tmmsm01_main["MAT_ACT_WT"];
				tmmsm35["MAT_TUBE"] = tmmsm01_main["MAT_NUM"];
				tmmsm35["OP_DIV"] = "4";//1：铸坯分段；2：方坯拆批；3：方坯并批；4：方坯组批
				tmmsm35.Insert();
			}

			//Log::Trace("", __FUNCTION__, "调用物料跟踪-被组材料删除 tmmsm01.MAT_NO		= [{0}]", (const char*)tmmsm01["MAT_NO"].ToString());

			#if defined _SYS_MMS || defined _SYS_MES
				if (tmmsm01["ORDER_NO"].ToString().Trim().GetLength() > 0)
				{

					/* 被并材料 */
					tpmof03["EVENT_ID"] = "5J";
					tpmof03["SYSTEM_ID"] = "MMSM";
					tpmof03["MAT_STATUS"] = tmmsm01["MAT_STATUS"];
					tpmof03["FUNC_ID"] = "f_mmsm80";
					tpmof03["ORDER_NO"] = tmmsm01["ORDER_NO"];
					tpmof03["WHOLE_BACKLOG_NO"] = tmmsm01["WHOLE_BACKLOG_NO"];
					tpmof03["WHOLE_BACKLOG"] = tmmsm01["WHOLE_BACKLOG"];
					tpmof03["WHOLE_BACKLOG_SEQ"] = tmmsm01["NEXT_WHOLE_BACKLOG_SEQ"];
					tpmof03["WHOLE_BACKLOG_CODE"] = tmmsm01["NEXT_WHOLE_BACKLOG_CODE"];
					tpmof03["PREV_MAT_STATUS"] = tmmsm01["MAT_STATUS"];
					tpmof03["MAT_NO"] = tmmsm01["MAT_NO"];
					tpmof03["WT"] = 0;
					tpmof03["NUM"] = 0;
					tpmof03["PREV_WT"] = tmmsm01["MAT_ACT_WT"];
					tpmof03["PREV_NUM"] = 1;
					if (tmmsm01["PLAN_NO"].ToString().Trim().GetLength() > 0)
					{
						tpmof03["IF_PLAN"] = "1";
					}
					else
					{
						tpmof03["IF_PLAN"] = "0";
					}
					tpmof03.MergeTo(bcls_rec->Tables["PMOF99"], false);
				}
			#endif  

			bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM32";	//材料被并批删除
			bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "00";
			bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
			if (funcId == "")
			{
				bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = s.svc_name;
			}
			else
			{
				bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = funcId;
			}
			bcls_rec->Tables["MM0099"].Rows[0]["EVENT_DESC"] = "组批材料删除";
			bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm01["MAT_NO"];

			/* 调用物料跟踪-被并材料删除 */
			doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}
			
		}
		#pragma endregion  

		tmmsm35["MAT_NO"] = aimMatNo;
		tmmsm35["MAT_TUBE"] = matTube;
		tmmsm35["MAT_WT"] = matActWt;
		tmmsm35.Update("MAT_TUBE,MAT_WT", "PROD_SEQ_NO,MAT_NO");

		#pragma region  处理母材料信息
		//Log::Trace("", __FUNCTION__, "新增母材料信息 tmmsm01_main.MAT_NO		= [{0}]",tmmsm01_main["MAT_NO"].ToString());
		//Log::Trace("", __FUNCTION__, "新增母材料信息 aimMatNo		= [{0}]", aimMatNo);


		/* 修改母材料支数等值 */
		tmmsm01_main["REC_REVISOR"] = s.userid;
		tmmsm01_main["REC_REVISE_TIME"] = datetime;

		tmmsm01_main["MAT_NO_OLD"] = tmmsm01_main["MAT_NO"];
		tmmsm01_main["OLD_HEAT_NO"] = tmmsm01_main["HEAT_NO"];
		tmmsm01_main["OLD_PONO"] = tmmsm01_main["PONO"];

		tmmsm01_main["MAT_NO"] = aimMatNo;
		tmmsm01_main["HEAT_NO"] = aimHeatNo;
		tmmsm01_main["PONO"] = aimPono;
		tmmsm01_main["MAT_NUM"] = matTube;
		tmmsm01_main["MAT_NUM_CUT"] = matTube;
		//tmmsm01_main.QTY = matTube;
		//tmmsm01_main["MAT_NUM"] = matTube;
		//tmmsm01_main.REMAIN_NUM = matTube;
		tmmsm01_main["MAT_ACT_WT"] = matActWt;
		tmmsm01_main["MAT_THEORY_WT"] = matTheoryWt;

        #if defined _SYS_MMS || defined _SYS_MES    //MMS层或MES系统部署时的数据块定义
		//生成物料材料跟踪号后4位流水号
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:				// DB2 数据库（未开Oracle兼容）
			sqlstr = " SELECT TO_CHAR(CURRENT TIMESTAMP, 'YYYYMMDDHH24MISSFF4') "
				" FROM SYSIBM.SYSDUMMY1  ";	//取20位系统时刻
			break;
		case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:				// MS SQL Server数据库
		case DB_KIND_ORACLE:	        // Oracle 数据库
		default:
			sqlstr = "SELECT TO_CHAR(SYSDATE,'YYYYMMDDHH24MISS')||LPAD(TO_CHAR(RESUME_SEQ_NO_TMMSM96.NEXTVAL),4,'0') "
				"  FROM DUAL ";	//6位前补零
			break;
		}
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.Parameters.Clear();
		cmd_inq.ExecuteReader();
		if (cmd_inq.Read())
		{
			tmmsm01_main["MAT_TRACK_NO"] = cmd_inq.GetString(1);
		}
		cmd_inq.Close();
        #endif

		sqlstr = "tmmsm01_main.Insert()";
		tmmsm01_main.Insert();


		//Log::Trace("", __FUNCTION__, "主材料抛物料跟踪履历 tmmsm01_main.MAT_NO		= [{0}]", (const char*)tmmsm01_main["MAT_NO"].ToString());
		/* 主材料抛物料跟踪履历 */
		bcls_rec->Tables["MM0099"].Rows.Clear();
		bcls_rec->Tables["MM0099"].Rows.Add();
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_ID"] = "MM31";
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_LINE_TYPE"] = "00";
		bcls_rec->Tables["MM0099"].Rows[0]["SYSTEM_ID"] = "MMSM";
		if (funcId == "")
		{
			bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = s.svc_name;
		}
		else
		{
			bcls_rec->Tables["MM0099"].Rows[0]["FUNC_ID"] = funcId;
		}
		bcls_rec->Tables["MM0099"].Rows[0]["EVENT_DESC"] = "组批母材料形成";
		bcls_rec->Tables["MM0099"].Rows[0]["MAT_NO"] = tmmsm01_main["MAT_NO"];
		doFlag = f_mmsm99(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

		#if defined _SYS_MMS || defined _SYS_MES
		//如果母材料是合同材，则要抛合同跟踪
		if (tmmsm01_main["ORDER_NO"].ToString().Trim().GetLength() > 0)
		{
			/* 母材料总量修正 */
			tpmof03["EVENT_ID"] = "54";
			tpmof03["SYSTEM_ID"] = "MMSM";
			tpmof03["MAT_STATUS"] = tmmsm01_main["MAT_STATUS"];
			tpmof03["FUNC_ID"] = "f_mmsm80";
			tpmof03["ORDER_NO"] = tmmsm01_main["ORDER_NO"];
			tpmof03["WHOLE_BACKLOG_NO"] = tmmsm01_main["WHOLE_BACKLOG_NO"];
			tpmof03["WHOLE_BACKLOG"] = tmmsm01_main["WHOLE_BACKLOG"];
			tpmof03["WHOLE_BACKLOG_SEQ"] = tmmsm01_main["NEXT_WHOLE_BACKLOG_SEQ"];
			tpmof03["WHOLE_BACKLOG_CODE"] = tmmsm01_main["NEXT_WHOLE_BACKLOG_CODE"];
			tpmof03["PREV_MAT_STATUS"] = tmmsm01_main["MAT_STATUS"];
			if (tmmsm01_main["PLAN_NO"].ToString().Trim().GetLength() > 0)
			{
				tpmof03["IF_PLAN"] = "1";
			}
			else
			{
				tpmof03["IF_PLAN"] = "0";
			}
			tpmof03["MAT_NO"] = tmmsm01_main["MAT_NO"];
			tpmof03["PREV_WT"] = v_main_mat_wt;
			tpmof03["WT"] = tmmsm01_main["MAT_ACT_WT"];
			tpmof03["NUM"] = 1;
			tpmof03.MergeTo(bcls_rec->Tables["PMOF99"], false);

			for (int i = 0; i < bcls_rec->Tables["PMOF99"].Rows.get_Count(); i++)
			{
				//Log::Trace("", __FUNCTION__, " bcls_rec->Tables['PMOF99'].Rows[i]['MAT_NO']		= [{0}]", bcls_rec->Tables["PMOF99"].Rows[i]["MAT_NO"].ToString());
				//Log::Trace("", __FUNCTION__, " bcls_rec->Tables['PMOF99'].Rows[i]['EVENT_ID']		= [{0}]", bcls_rec->Tables["PMOF99"].Rows[i]["EVENT_ID"].ToString());
			}

			doFlag = f_pmof99_v3(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		}
		#endif  




		#if defined _SYS_PES || defined _SYS_MES
		/*	if (ymque == "1")
		{
		if (bcls_rec->Tables.Contains("WM00QUE") == false)
		{
		bcls_rec->Tables.Add("WM00QUE");
		}
		if (bcls_rec->Tables["WM00QUE"].Columns.Contains("MAT_NO") == false)
		{
		bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_NO");
		}
		if (bcls_rec->Tables["WM00QUE"].Columns.Contains("MAT_LINE_TYPE") == false)
		{
		bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_LINE_TYPE");
		}
		if (bcls_rec->Tables["WM00QUE"].Columns.Contains("MAT_KIND") == false)
		{
		bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "MAT_KIND");
		}
		if (bcls_rec->Tables["WM00QUE"].Columns.Contains("STOCK_OPER_ORDER") == false)
		{
		bcls_rec->Tables["WM00QUE"].Columns.Add(DT_STRING, "STOCK_OPER_ORDER");
		}

		if (bcls_rec->Tables["WM00QUE"].Rows.get_Count() <= 0)
		{
		bcls_rec->Tables["WM00QUE"].Rows.Add();
		}
		bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_NO"] = tmmsm01_main["MAT_NO"];
		bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_KIND"] = "SM";
		bcls_rec->Tables["WM00QUE"].Rows[0]["MAT_LINE_TYPE"] = tmmsm01_main["MAT_LINE_TYPE"];
		bcls_rec->Tables["WM00QUE"].Rows[0]["STOCK_OPER_ORDER"] = "1L";

		doFlag = f_wm00_queue(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
		throw CApplicationException(-1, s.msg, log.Location);
		}
		}*/
		#endif
		#pragma endregion

#if  defined _SYS_PES
		#pragma region 电文处理
		if (ifTcSnd == "1")
		{
			tcBacklog = "MMSM3H";
			if (tcBacklog != "")
			{
				blkNum = bcls_rec->Tables.IndexOf("MMSMSND");
				if (blkNum < 0)
				{
					bcls_rec->Tables.Add("MMSMSND");
				}
				if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("TC_BACKLOG"))
				{
					bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "TC_BACKLOG");
				}
				if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("EVENT_ID"))
				{
					bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "EVENT_ID");
				}
				if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("REC_CREATOR"))
				{
					bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "REC_CREATOR");
				}
				if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("REC_CREATE_TIME"))
				{
					bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "REC_CREATE_TIME");
				}
				if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("AIM_MAT_NO"))
				{
					bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "AIM_MAT_NO");
				}
				if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("AIM_HEAT_NO"))
				{
					bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "AIM_HEAT_NO");
				}
				if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("AIM_PONO"))
				{
					bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "AIM_PONO");
				}
				if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("MAT_NO"))
				{
					bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "MAT_NO");
				}
				if (!bcls_rec->Tables["MMSMSND"].Columns.Contains("MAIN_MAT_FLAG"))
				{
					bcls_rec->Tables["MMSMSND"].Columns.Add(DT_STRING, "MAIN_MAT_FLAG");
				}
				for (int i = 0; i < bcls_rec->Tables["MMSM80"].Rows.get_Count(); i++)
				{
					bcls_rec->Tables["MMSMSND"].Rows.Add();
					bcls_rec->Tables["MMSMSND"].Rows[i]["TC_BACKLOG"] = tcBacklog;
					bcls_rec->Tables["MMSMSND"].Rows[i]["EVENT_ID"] = "M80A";
					bcls_rec->Tables["MMSMSND"].Rows[i]["REC_CREATOR"] = s.userid;
					bcls_rec->Tables["MMSMSND"].Rows[i]["REC_CREATE_TIME"] = datetime;
					if (bcls_rec->Tables["MMSM80"].Rows[i]["MAIN_MAT_FLAG"].ToString().Trim() == "1")
					{
						bcls_rec->Tables["MMSMSND"].Rows[0]["AIM_MAT_NO"] = bcls_rec->Tables["MMSM80"].Rows[i]["AIM_MAT_NO"].ToString().Trim();
						bcls_rec->Tables["MMSMSND"].Rows[0]["AIM_HEAT_NO"] = bcls_rec->Tables["MMSM80"].Rows[i]["AIM_HEAT_NO"].ToString().Trim();
						bcls_rec->Tables["MMSMSND"].Rows[0]["AIM_PONO"] = bcls_rec->Tables["MMSM80"].Rows[i]["AIM_PONO"].ToString().Trim();
						bcls_rec->Tables["MMSMSND"].Rows[i]["MAIN_MAT_FLAG"] = "1";
					}
					bcls_rec->Tables["MMSMSND"].Rows[i]["MAT_NO"] = bcls_rec->Tables["MMSM80"].Rows[i]["MAT_NO"].ToString().Trim();
				}
				//doFlag = f_mmsm009a_snd(bcls_rec, bcls_ret, conn);

				if (doFlag < 0)
				{
					throw CApplicationException(doFlag, s.msg, log.Location);
				}
				//Log::Trace("", __FUNCTION__, "成功调用发送并批电文：tmmsm01_main["MAT_NO"] = [%s] ", (const char*)tmmsm01_main["MAT_NO"].ToString());
			}
		}
		#pragma endregion
#endif
		/* 设置返回块名 */
		blkNum = bcls_ret->Tables.IndexOf("MMSM80");				//设置返回块名
		if (blkNum < 0)
		{
			bcls_ret->Tables.Add("MMSM80");
			bcls_ret->Tables["MMSM80"].Columns.Add(DT_STRING, "MAT_NO");
			bcls_ret->Tables["MMSM80"].Columns.Add(DT_DECIMAL, "MAT_NUM");
			bcls_ret->Tables["MMSM80"].Columns.Add(DT_DECIMAL, "MAT_ACT_WT");
			bcls_ret->Tables["MMSM80"].Rows.Add();
		}
		bcls_ret->Tables["MMSM80"].Rows[0]["MAT_NO"] = tmmsm01_main["MAT_NO"];
		bcls_ret->Tables["MMSM80"].Rows[0]["MAT_NUM"] = tmmsm01_main["MAT_NUM"];
		bcls_ret->Tables["MMSM80"].Rows[0]["MAT_ACT_WT"] = tmmsm01_main["MAT_ACT_WT"];

	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}],请联系开发人员", arguments, 1);
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


