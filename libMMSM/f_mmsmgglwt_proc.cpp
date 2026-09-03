
#include "stdafx.h"
#include "epex.h"




BM2_FUNCTION_EXPORT


int f_mmsmgglwt_proc(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 静态变量定义 ***** */
	int 	doFlag = 0;
	int 	ret = 0;
	int 	fetchRowCount = 0;
	CString	lpsz_tc_no = "";
	int   blkNum = 0;
	CString v_time = "";
	int  v_count1 = 0;
	int  v_count2 = 0;
	int sqlid = 0;
	CString sqlstr = "";
	CString eventDesc = "";
	CString wtOrder = "";
	CDecimal totalNetWt = 0;
	CDecimal totalActWt = 0;
	CString dev_code = " ";
	CString dev_code12 = " ";
	CString heat_no = " ";
	CString m_heat_no = " ";
	CDecimal wt = 0;
	CDecimal actresult = 0;
	CDecimal actresult_avg = 0;
	CDecimal wt_sum = 0;
	CDbCommand cmd_sql(conn);
	CDbCommand cmd_inqj(conn);
	CDbCommand cmd_inqy(conn);
	CDbCommand cmd_inqx(conn);
	CDbCommand cmd_inqgylx(conn);
	CDbCommand cmd_inqgylx01(conn);
	CDbCommand cmd_inqhl(conn);
	CString table_name = " ";

	CDbCommand cmd_inq(conn);
	CDbCommand cmd_inq12(conn);
	CModel tmmsm19("TMMSM19");
	CModel tmmsm27("TMMSM27");
	CModel tmmsm21("TMMSM21");
	CModel tmmsm20("TMMSM20");
	CModel tmmsm24("TMMSM24");
	CModel tmmsm26("TMMSM26");
	CModel tmmsm23("TMMSM23");
	CModel tmmsm25("TMMSM25");
	CModel tmmsm31("TMMSM31");
	CModel tmmsm33("TMMSM33");
	CModel tmmsm14("TMMSM14");
	CModel tmmsm2a("TMMSM2A");
	CModel tmmsmgy06("TMMSMGY06");
	CModel tmmsmhl("TMMSMHL");
	CString affirm_flag = " ";
	CString l2_proc_no = " ";
	CString proc_no = " ";
	CString flag = " ";
	try
	{
		//判断是回炉调还是工艺路线确认调,1表示回炉，2表示工艺路线
		flag = bcls_rec->Tables[0].Rows[0]["FLAG"].ToString().Trim();
		if (flag == "1"){
			dev_code = bcls_rec->Tables[0].Rows[0]["DEV_CODE"].ToString().Substring(0, 1);
			Log::Trace("", __FUNCTION__, "dev_code=[{0}]", dev_code);
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
			m_heat_no = bcls_rec->Tables[0].Rows[0]["MB_HEAT_NO"].ToString().Trim();
			wt = bcls_rec->Tables[0].Rows[0]["WTS"].ToDecimal();
			tmmsmgy06["HEAT_NO"] = heat_no;
			if (tmmsmgy06.QueryCount("HEAT_NO") > 0){
				CDecimal tc_send_flag = 0;
				CDecimal duration_time = 0;
				cmd_inqgylx01.SetCommandText(" select AFFIRM_FLAG,TC_SEND_FLAG,DURATION_TIME from TMMSMGY06 WHERE HEAT_NO='" + heat_no + "' ");
				cmd_inqgylx01.ExecuteReader();
				while (cmd_inqgylx01.Read())
				{
					affirm_flag = cmd_inqgylx01.GetString(1);
					tc_send_flag = cmd_inqgylx01.GetDecimal(2);
					duration_time = cmd_inqgylx01.GetDecimal(3);
					//如果是1表示已经确认工艺路线
					if (affirm_flag == "1"){
						if (wt != 0 && m_heat_no != " "){
							//根据传过来的工位判断上一工序是哪个表
							if (dev_code == "Z"){
								table_name = "TMMSM19";
							}
							else if (dev_code == "A"){
								table_name = "TMMSM27";
							}
							else if (dev_code == "E"){
								table_name = "TMMSM20";
							}
							else if (dev_code == "B"){
								table_name = "TMMSM21";
							}
							/*else if (dev_code == "F"){
							table_name = "TMMSM24";
							}*/
							else if (dev_code == "R"){
								table_name = "TMMSM23";
							}
							/*else if (dev_code == "S"){
							table_name = "tmmsm26";
							}*/
							else if (dev_code == "V")
							{
								table_name = "tmmsm25";
							}
							Log::Trace("", __FUNCTION__, "table_name=[{0}]", table_name);
							//根据表去查该工序的实绩重量
							cmd_sql.SetCommandText(" SELECT ACTRESULT FROM " + table_name + "  where HEAT_NO='" + heat_no + "' ");
							cmd_sql.ExecuteReader();
							while (cmd_sql.Read())
							{
								actresult = cmd_sql.GetDecimal(1);
								if (actresult != 0){
									//根据计划传过来的重量/实绩重量=每工序要减去的值
									actresult_avg = wt / actresult;
									Log::Trace("", __FUNCTION__, "actresult_avg=[{0}]", actresult_avg);
									//获取该熔炼号有多少导工序
									cmd_inq12.SetCommandText(" select DEV_CODE from TPSSM12 WHERE HEAT_NO='" + heat_no + "' ");
									cmd_inq12.ExecuteReader();
									while (cmd_inq12.Read())
									{
										dev_code12 = cmd_inq12.GetString(1).Substring(0, 1);
										Log::Trace("", __FUNCTION__, "dev_code12=[{0}]", dev_code12);
										if (dev_code12 == "Z"){
											//根据表去查该工序的实绩重量
											CDecimal j_time = 0;
											CDecimal time = 0;
											cmd_inq.SetCommandText(" SELECT * FROM TMMSM19  where HEAT_NO='" + heat_no + "' ");
											cmd_inq.ExecuteReader();
											while (cmd_inq.Read())
											{
												cmd_inq.Fetch(tmmsm19);
												cmd_inq.Fetch(tmmsmgy06);
												tmmsmgy06["HEAT_NO"] = tmmsm19["HEAT_NO"];
												tmmsmgy06["L2_PROC_NO"] = tmmsm19["L2_PROC_NO"];
												//PROC_NO
												tmmsmgy06["PROC_NO"] = tmmsm19["PROC_NO"];
												tmmsmgy06["DEV_CODE"] = tmmsm19["DEV_CODE"];
												if (tmmsmgy06.Query("HEAT_NO,PROC_NO,DEV_CODE")){
													CDecimal together_time = 0;
													together_time = duration_time;
													time = duration_time;
													j_time = time  * actresult_avg;
													wt_sum = wt_sum + j_time;
													tmmsmgy06["DURATION_TIME"] = time - j_time;
													tmmsmgy06["DURATION_TIME"] = tmmsmgy06["DURATION_TIME"].ToDecimal().Round(0);
													if (tc_send_flag == 2){
														tmmsmgy06["TOGETHER_TIME"] = together_time;
													}
													else{
														tmmsmgy06["TOGETHER_TIME"] = "0";
													}
													tmmsmgy06["DURATION_TIME"] = tmmsmgy06["DURATION_TIME"].ToDecimal().Round(0);
													tmmsmgy06.Update("DURATION_TIME,TOGETHER_TIME", "HEAT_NO,PROC_NO,DEV_CODE");
												}
											}
											cmd_inq.Close();
										}
										else if (dev_code12 == "A"){
											//根据表去查该工序的实绩重量
											CDecimal j_time = 0;
											CDecimal time = 0;
											cmd_inq.SetCommandText(" SELECT * FROM TMMSM27  where HEAT_NO='" + heat_no + "' ");
											cmd_inq.ExecuteReader();
											if (cmd_inq.Read())
											{
												cmd_inq.Fetch(tmmsm27);
												cmd_inq.Fetch(tmmsmgy06);
												time = duration_time;
												j_time = time  * actresult_avg;
												wt_sum = wt_sum + j_time;
												tmmsmgy06["HEAT_NO"] = tmmsm27["HEAT_NO"];
												tmmsmgy06["L2_PROC_NO"] = tmmsm27["L2_PROC_NO"];
												tmmsmgy06["DEV_CODE"] = tmmsm27["DEV_CODE"];
												tmmsmgy06["PROC_NO"] = tmmsm27["PROC_NO"];
												if (tmmsmgy06.Query("HEAT_NO,PROC_NO,DEV_CODE")){
													tmmsmgy06["DURATION_TIME"] = time - j_time;
													if (tc_send_flag == 2){
														tmmsmgy06["TOGETHER_TIME"] = duration_time;
													}
													else{
														tmmsmgy06["TOGETHER_TIME"] = "0";
													}
													tmmsmgy06["DURATION_TIME"] = tmmsmgy06["DURATION_TIME"].ToDecimal().Round(0);
													tmmsmgy06.Update("DURATION_TIME,TOGETHER_TIME", "HEAT_NO,PROC_NO,DEV_CODE");
												}
											}
											cmd_inq.Close();
										}
										else if (dev_code12 == "E"){
											CDecimal j_time = 0;
											CDecimal time = 0;
											cmd_inq.SetCommandText(" SELECT * FROM TMMSM20  where HEAT_NO='" + heat_no + "' ");
											cmd_inq.ExecuteReader();
											while (cmd_inq.Read())
											{
												cmd_inq.Fetch(tmmsm20);
												cmd_inq.Fetch(tmmsmgy06);
												time = duration_time;
												j_time = time  * actresult_avg;
												wt_sum = wt_sum + j_time;
												tmmsmgy06["HEAT_NO"] = tmmsm20["HEAT_NO"];
												tmmsmgy06["L2_PROC_NO"] = tmmsm20["L2_PROC_NO"];
												tmmsmgy06["DEV_CODE"] = tmmsm20["DEV_CODE"];
												tmmsmgy06["PROC_NO"] = tmmsm20["PROC_NO"];
												if (tmmsmgy06.Query("HEAT_NO,PROC_NO,DEV_CODE")){
													tmmsmgy06["DURATION_TIME"] = time - j_time;
													if (tc_send_flag == 2){
														tmmsmgy06["TOGETHER_TIME"] = duration_time;
													}
													else{
														tmmsmgy06["TOGETHER_TIME"] = "0";
													}
													tmmsmgy06["DURATION_TIME"] = tmmsmgy06["DURATION_TIME"].ToDecimal().Round(0);
													tmmsmgy06.Update("DURATION_TIME,TOGETHER_TIME", "HEAT_NO,PROC_NO,DEV_CODE");
												}
											}
											cmd_inq.Close();
										}
										else if (dev_code12 == "B"){
											CDecimal j_time = 0;
											CDecimal time = 0;
											cmd_inq.SetCommandText(" SELECT * FROM TMMSM21  where HEAT_NO='" + heat_no + "' ");
											cmd_inq.ExecuteReader();
											while (cmd_inq.Read())
											{
												cmd_inq.Fetch(tmmsm21);
												cmd_inq.Fetch(tmmsmgy06);
												time = duration_time;
												j_time = time  * actresult_avg;
												wt_sum = wt_sum + j_time;
												tmmsmgy06["HEAT_NO"] = tmmsm21["HEAT_NO"];
												tmmsmgy06["L2_PROC_NO"] = tmmsm21["L2_PROC_NO"];
												tmmsmgy06["DEV_CODE"] = tmmsm21["DEV_CODE"];
												tmmsmgy06["PROC_NO"] = tmmsm21["PROC_NO"];
												tmmsmgy06["TOGETHER_TIME"] = j_time;
												if (tmmsmgy06.Query("HEAT_NO,PROC_NO,DEV_CODE")){
													tmmsmgy06["DURATION_TIME"] = time - j_time;
													tmmsmgy06["DURATION_TIME"] = tmmsmgy06["DURATION_TIME"].ToDecimal().Round(0);
													if (tc_send_flag == 2){
														tmmsmgy06["TOGETHER_TIME"] = duration_time;
													}
													else{
														tmmsmgy06["TOGETHER_TIME"] = "0";
													}
													tmmsmgy06.Update("DURATION_TIME,TOGETHER_TIME", "HEAT_NO,PROC_NO,DEV_CODE");
												}
											}
											cmd_inq.Close();
										}
										else if (dev_code12 == "F"){
											CDecimal j_time = 0;
											CDecimal time = 0;
											cmd_inq.SetCommandText(" SELECT * FROM TMMSM24  where HEAT_NO='" + heat_no + "' ");
											cmd_inq.ExecuteReader();
											if (cmd_inq.Read())
											{
												cmd_inq.Fetch(tmmsm24);
												cmd_inq.Fetch(tmmsmgy06);
												time = duration_time;
												j_time = time  * actresult_avg;
												wt_sum = wt_sum + j_time;
												tmmsmgy06["HEAT_NO"] = tmmsm24["HEAT_NO"];
												tmmsmgy06["L2_PROC_NO"] = tmmsm24["L2_PROC_NO"];
												tmmsmgy06["DEV_CODE"] = tmmsm24["DEV_CODE"];
												tmmsmgy06["PROC_NO"] = tmmsm24["PROC_NO"];
												tmmsmgy06["TOGETHER_TIME"] = j_time;
												if (tmmsmgy06.Query("HEAT_NO,PROC_NO,DEV_CODE")){
													tmmsmgy06["DURATION_TIME"] = time - j_time;
													tmmsmgy06["DURATION_TIME"] = tmmsmgy06["DURATION_TIME"].ToDecimal().Round(0);
													if (tc_send_flag == 2){
														tmmsmgy06["TOGETHER_TIME"] = duration_time;
													}
													else{
														tmmsmgy06["TOGETHER_TIME"] = "0";
													}
													tmmsmgy06.Update("DURATION_TIME,TOGETHER_TIME", "HEAT_NO,PROC_NO,DEV_CODE");
												}
											}
											cmd_inq.Close();
										}
										else if (dev_code12 == "R"){
											CDecimal j_time = 0;
											CDecimal time = 0;
											cmd_inq.SetCommandText(" SELECT * FROM TMMSM23  where HEAT_NO='" + heat_no + "' ");
											cmd_inq.ExecuteReader();
											if (cmd_inq.Read())
											{
												cmd_inq.Fetch(tmmsm23);
												cmd_inq.Fetch(tmmsmgy06);
												time = duration_time;
												j_time = time  * actresult_avg;
												wt_sum = wt_sum + j_time;
												tmmsmgy06["HEAT_NO"] = tmmsm23["HEAT_NO"];
												tmmsmgy06["L2_PROC_NO"] = tmmsm23["L2_PROC_NO"];
												tmmsmgy06["DEV_CODE"] = tmmsm23["DEV_CODE"];
												tmmsmgy06["PROC_NO"] = tmmsm24["PROC_NO"];
												tmmsmgy06["TOGETHER_TIME"] = j_time;
												if (tmmsmgy06.Query("HEAT_NO,PROC_NO,DEV_CODE")){
													tmmsmgy06["DURATION_TIME"] = time - j_time;
													tmmsmgy06["DURATION_TIME"] = tmmsmgy06["DURATION_TIME"].ToDecimal().Round(0);
													if (tc_send_flag == 2){
														tmmsmgy06["TOGETHER_TIME"] = duration_time;
													}
													else{
														tmmsmgy06["TOGETHER_TIME"] = "0";
													}
													tmmsmgy06.Update("DURATION_TIME,TOGETHER_TIME", "HEAT_NO,PROC_NO,DEV_CODE");
												}
											}
											cmd_inq.Close();
										}
										else if (dev_code12 == "S"){
											CDecimal j_time = 0;
											CDecimal time = 0;
											cmd_inq.SetCommandText(" SELECT * FROM TMMSM26  where HEAT_NO='" + heat_no + "' ");
											cmd_inq.ExecuteReader();
											if (cmd_inq.Read())
											{
												cmd_inq.Fetch(tmmsm26);
												cmd_inq.Fetch(tmmsmgy06);
												time = duration_time;
												j_time = time  * actresult_avg;
												wt_sum = wt_sum + j_time;
												tmmsmgy06["HEAT_NO"] = tmmsm26["HEAT_NO"];
												tmmsmgy06["L2_PROC_NO"] = tmmsm26["L2_PROC_NO"];
												tmmsmgy06["DEV_CODE"] = tmmsm26["DEV_CODE"];
												tmmsmgy06["TOGETHER_TIME"] = j_time;
												tmmsmgy06["PROC_NO"] = tmmsm26["PROC_NO"];
												if (tmmsmgy06.Query("HEAT_NO,PROC_NO,DEV_CODE")){
													tmmsmgy06["DURATION_TIME"] = time - j_time;
													tmmsmgy06["DURATION_TIME"] = tmmsmgy06["DURATION_TIME"].ToDecimal().Round(0);
													if (tc_send_flag == 2){
														tmmsmgy06["TOGETHER_TIME"] = duration_time;
													}
													else{
														tmmsmgy06["TOGETHER_TIME"] = "0";
													}
													tmmsmgy06.Update("DURATION_TIME,TOGETHER_TIME", "HEAT_NO,PROC_NO,DEV_CODE");
												}
											}
											cmd_inq.Close();
										}
										else if (dev_code12 == "V")
										{
											CDecimal j_time = 0;
											CDecimal time = 0;
											cmd_inq.SetCommandText(" SELECT * FROM TMMSM25  where HEAT_NO='" + heat_no + "' ");
											cmd_inq.ExecuteReader();
											if (cmd_inq.Read())
											{
												cmd_inq.Fetch(tmmsm25);
												cmd_inq.Fetch(tmmsmgy06);
												time = duration_time;
												j_time = time  * actresult_avg;
												wt_sum = wt_sum + j_time;
												tmmsmgy06["HEAT_NO"] = tmmsm25["HEAT_NO"];
												tmmsmgy06["L2_PROC_NO"] = tmmsm25["L2_PROC_NO"];
												tmmsmgy06["DEV_CODE"] = tmmsm25["DEV_CODE"];
												tmmsmgy06["PROC_NO"] = tmmsm25["PROC_NO"];
												tmmsmgy06["TOGETHER_TIME"] = j_time;
												if (tmmsmgy06.Query("HEAT_NO,PROC_NO,DEV_CODE")){
													tmmsmgy06["DURATION_TIME"] = time - j_time;
													tmmsmgy06["DURATION_TIME"] = tmmsmgy06["DURATION_TIME"].ToDecimal().Round(0);
													if (tc_send_flag == 2){
														tmmsmgy06["TOGETHER_TIME"] = duration_time;
													}
													else{
														tmmsmgy06["TOGETHER_TIME"] = "0";
													}
													tmmsmgy06.Update("DURATION_TIME,TOGETHER_TIME,TOGETHER_TIME", "HEAT_NO,PROC_NO,DEV_CODE");
												}
												else{
													tmmsmgy06["TOGETHER_TIME"] = j_time;
													tmmsmgy06.Insert();
												}
											}
											cmd_inq.Close();
										}
									}
									cmd_inq12.Close();
									////根据要添加的熔炼号去加重量
									//cmd_inqj.SetCommandText(" select DEV_CODE from TPSSM12 WHERE HEAT_NO='" + m_heat_no + "' ");
									////获取条数
									//CDecimal count = cmd_inqj.ExecuteScalar();
									////获取每工序加多少重量
									//CDecimal avg = wt_sum / count;
									//cmd_inqj.ExecuteReader();
									//while (cmd_inqj.Read())
									//{
									//	CString dev_codex = cmd_inq12.GetString(1).Substring(0, 1);
									//	if (dev_codex == "Z"){
									//		//根据表去查该工序的实绩重量
									//		cmd_inq.SetCommandText(" SELECT * FROM TMMSM19  where HEAT_NO='" + m_heat_no + "' ");
									//		cmd_inq.ExecuteReader();
									//		if (cmd_inq.Read())
									//		{
									//			cmd_inq.Fetch(tmmsm19);
									//			tmmsm19["ACTRESULT"] = tmmsm19["ACTRESULT"].ToDecimal() + avg;
									//			tmmsm19.Update("ACTRESULT", "HEAT_NO");
									//		}
									//		cmd_inq.Close();
									//	}
									//	else if (dev_codex == "A"){
									//		cmd_inq.SetCommandText(" SELECT * FROM TMMSM27  where HEAT_NO='" + m_heat_no + "' ");
									//		cmd_inq.ExecuteReader();
									//		if (cmd_inq.Read())
									//		{
									//			cmd_inq.Fetch(tmmsm27);
									//			tmmsm27["ACTRESULT"] = tmmsm27["ACTRESULT"].ToDecimal() + avg;
									//			tmmsm27.Update("ACTRESULT", "HEAT_NO");
									//		}
									//		cmd_inq.Close();
									//	}
									//	else if (dev_codex == "E"){
									//		cmd_inq.SetCommandText(" SELECT * FROM TMMSM20  where HEAT_NO='" + m_heat_no + "' ");
									//		cmd_inq.ExecuteReader();
									//		if (cmd_inq.Read())
									//		{
									//			cmd_inq.Fetch(tmmsm20);
									//			tmmsm20["ACTRESULT"] = tmmsm20["ACTRESULT"].ToDecimal() + avg;
									//			tmmsm20.Update("ACTRESULT", "HEAT_NO");
									//		}
									//		cmd_inq.Close();
									//	}
									//	else if (dev_codex == "B"){
									//		cmd_inq.SetCommandText(" SELECT * FROM TMMSM21  where HEAT_NO='" + m_heat_no + "' ");
									//		cmd_inq.ExecuteReader();
									//		if (cmd_inq.Read())
									//		{
									//			cmd_inq.Fetch(tmmsm21);
									//			tmmsm21["ACTRESULT"] = tmmsm21["ACTRESULT"].ToDecimal() + avg;
									//			tmmsm21.Update("ACTRESULT", "HEAT_NO");
									//		}
									//		cmd_inq.Close();
									//		
									//	}
									//	else if (dev_codex == "F"){
									//		cmd_inq.SetCommandText(" SELECT * FROM TMMSM24  where HEAT_NO='" + m_heat_no + "' ");
									//		cmd_inq.ExecuteReader();
									//		if (cmd_inq.Read())
									//		{
									//			cmd_inq.Fetch(tmmsm24);
									//			tmmsm24["ACTRESULT"] = tmmsm24["ACTRESULT"].ToDecimal() + avg;
									//			tmmsm24.Update("ACTRESULT", "HEAT_NO");
									//		}
									//		cmd_inq.Close();
									//	}
									//	else if (dev_codex == "R"){
									//		/*cmd_inq.SetCommandText(" SELECT * FROM TMMSM23  where HEAT_NO='" + m_heat_no + "' ");
									//		cmd_inq.ExecuteReader();
									//		if (cmd_inq.Read())
									//		{
									//			cmd_inq.Fetch(tmmsm23);
									//			tmmsm23["ACTRESULT"] = tmmsm23["ACTRESULT"].ToDecimal() + avg;
									//			tmmsm23.Update("ACTRESULT", "HEAT_NO");
									//		}
									//		cmd_inq.Close();*/
									//	}
									//	else if (dev_codex == "S"){
									//		/*cmd_inq.SetCommandText(" SELECT * FROM TMMSM26  where HEAT_NO='" + m_heat_no + "' ");
									//		cmd_inq.ExecuteReader();
									//		if (cmd_inq.Read())
									//		{
									//			cmd_inq.Fetch(tmmsm24);
									//			tmmsm24["ACTRESULT"] = tmmsm24["ACTRESULT"].ToDecimal() + avg;
									//			tmmsm24.Update("ACTRESULT", "HEAT_NO");
									//		}
									//		cmd_inq.Close();*/
									//	}
									//	else if (dev_codex == "V")
									//	{
									//		cmd_inq.SetCommandText(" SELECT * FROM TMMSM25  where HEAT_NO='" + m_heat_no + "' ");
									//		cmd_inq.ExecuteReader();
									//		if (cmd_inq.Read())
									//		{
									//			cmd_inq.Fetch(tmmsm25);
									//			tmmsm25["ACTRESULT"] = tmmsm25["ACTRESULT"].ToDecimal() + avg;
									//			tmmsm25.Update("ACTRESULT", "HEAT_NO");
									//		}
									//		cmd_inq.Close();
									//	}
									//}
									cmd_inqj.Close();
								}
							}
							cmd_sql.Close();
							Log::Trace("", __FUNCTION__, "actresult=[{0}]", actresult);
							
						}
					}
					else{
						Log::Trace("", __FUNCTION__, "未确认未下发_flag=[{0}]", heat_no);
					}
				}
				cmd_inqgylx01.Close();
				
			}
			else
			{
				if (wt != 0 && m_heat_no != " "){
					//根据传过来的工位判断上一工序是哪个表
					if (dev_code == "Z"){
						table_name = "TMMSM19";
					}
					else if (dev_code == "A"){
						table_name = "TMMSM27";
					}
					else if (dev_code == "E"){
						table_name = "TMMSM20";
					}
					else if (dev_code == "B"){
						table_name = "TMMSM21";
					}
					/*else if (dev_code == "F"){
					table_name = "TMMSM24";
					}*/
					else if (dev_code == "R"){
						table_name = "TMMSM23";
					}
					/*else if (dev_code == "S"){
					table_name = "tmmsm26";
					}*/
					else if (dev_code == "V")
					{
						table_name = "tmmsm25";
					}
					Log::Trace("", __FUNCTION__, "table_name=[{0}]", table_name);
					//根据表去查该工序的实绩重量
					cmd_sql.SetCommandText(" SELECT ACTRESULT FROM " + table_name + "  where HEAT_NO='" + heat_no + "' ");
					cmd_sql.ExecuteReader();
					if (cmd_sql.Read())
					{
						actresult = cmd_sql.GetDecimal(1);
					}
					cmd_sql.Close();
					Log::Trace("", __FUNCTION__, "actresult=[{0}]", actresult);
					if (actresult != 0){
						//根据计划传过来的重量/实绩重量=每工序要减去的值
						actresult_avg = wt / actresult;
						Log::Trace("", __FUNCTION__, "actresult_avg=[{0}]", actresult_avg);
						//获取该熔炼号有多少导工序
						cmd_inq12.SetCommandText(" select DEV_CODE from TPSSM12 WHERE HEAT_NO='" + heat_no + "' ");
						cmd_inq12.ExecuteReader();
						while (cmd_inq12.Read())
						{
							dev_code12 = cmd_inq12.GetString(1).Substring(0, 1);
							Log::Trace("", __FUNCTION__, "dev_code12=[{0}]", dev_code12);
							if (dev_code12 == "Z"){
								//根据表去查该工序的实绩重量
								CDecimal j_time = 0;
								CDecimal time = 0;
								cmd_inq.SetCommandText(" SELECT * FROM TMMSM19  where HEAT_NO='" + heat_no + "' ");
								cmd_inq.ExecuteReader();
								while (cmd_inq.Read())
								{
									cmd_inq.Fetch(tmmsm19);
									cmd_inq.Fetch(tmmsmhl);
									time = tmmsm19["END_TIME"].ToDecimal() - tmmsm19["START_TIME"].ToDecimal();
									j_time = time  * actresult_avg;
									wt_sum = wt_sum + j_time;
									tmmsmhl["HEAT_NO"] = tmmsm19["HEAT_NO"];
									tmmsmhl["L2_PROC_NO"] = tmmsm19["L2_PROC_NO"];
									tmmsmhl["DEV_CODE"] = tmmsm19["DEV_CODE"];
									tmmsmhl["TOGETHER_TIME"] = j_time/60;
									tmmsmhl["TOGETHER_TIME"] = tmmsmhl["TOGETHER_TIME"].ToDecimal().Round(0);
									tmmsmhl["PROC_NO"] = tmmsm19["PROC_NO"];
									if (tmmsmhl.Query("HEAT_NO,PROC_NO")){
										tmmsmhl.Update("TOGETHER_TIME","HEAT_NO,PROC_NO");
									}
									else{
										tmmsmhl.Insert();
									}
								}
								cmd_inq.Close();
							}
							else if (dev_code12 == "A"){
								//根据表去查该工序的实绩重量
								CDecimal j_time = 0;
								CDecimal time = 0;
								cmd_inq.SetCommandText(" SELECT * FROM TMMSM27  where HEAT_NO='" + heat_no + "' ");
								cmd_inq.ExecuteReader();
								while (cmd_inq.Read())
								{
									cmd_inq.Fetch(tmmsm27);
									cmd_inq.Fetch(tmmsmhl);
									time = tmmsm27["END_TIME"].ToDecimal() - tmmsm27["START_TIME"].ToDecimal();
									j_time = time  * actresult_avg;
									wt_sum = wt_sum + j_time;
									tmmsmhl["HEAT_NO"] = tmmsm27["HEAT_NO"];
									tmmsmhl["L2_PROC_NO"] = tmmsm27["L2_PROC_NO"];
									tmmsmhl["DEV_CODE"] = tmmsm27["DEV_CODE"];
									tmmsmhl["TOGETHER_TIME"] = j_time / 60;
									tmmsmhl["TOGETHER_TIME"] = tmmsmhl["TOGETHER_TIME"].ToDecimal().Round(0);
									tmmsmhl["PROC_NO"] = tmmsm27["PROC_NO"];
									if (tmmsmhl.Query("HEAT_NO,PROC_NO")){
										tmmsmhl.Update("TOGETHER_TIME", "HEAT_NO,PROC_NO");
									}
									else{
										tmmsmhl.Insert();
									}
								}
								cmd_inq.Close();
							}
							else if (dev_code12 == "E"){
								CDecimal j_time = 0;
								CDecimal time = 0;
								cmd_inq.SetCommandText(" SELECT * FROM TMMSM20  where HEAT_NO='" + heat_no + "' ");
								cmd_inq.ExecuteReader();
								if (cmd_inq.Read())
								{
									cmd_inq.Fetch(tmmsm20);
									cmd_inq.Fetch(tmmsmhl);
									time = tmmsm20["END_TIME"].ToDecimal() - tmmsm20["START_TIME"].ToDecimal();
									j_time = time  * actresult_avg;
									wt_sum = wt_sum + j_time;
									tmmsmhl["HEAT_NO"] = tmmsm20["HEAT_NO"];
									tmmsmhl["L2_PROC_NO"] = tmmsm20["L2_PROC_NO"];
									tmmsmhl["DEV_CODE"] = tmmsm20["DEV_CODE"];
									tmmsmhl["TOGETHER_TIME"] = j_time / 60;
									tmmsmhl["TOGETHER_TIME"] = tmmsmhl["TOGETHER_TIME"].ToDecimal().Round(0);
									tmmsmhl["PROC_NO"] = tmmsm20["PROC_NO"];
									if (tmmsmhl.Query("HEAT_NO,PROC_NO")){
										tmmsmhl.Update("TOGETHER_TIME", "HEAT_NO,PROC_NO");
									}
									else{
										tmmsmhl.Insert();
									}
									tmmsmhl.Insert();
								}
								cmd_inq.Close();
							}
							else if (dev_code12 == "B"){
								CDecimal j_time = 0;
								CDecimal time = 0;
								cmd_inq.SetCommandText(" SELECT * FROM TMMSM21  where HEAT_NO='" + heat_no + "' ");
								cmd_inq.ExecuteReader();
								if (cmd_inq.Read())
								{
									cmd_inq.Fetch(tmmsm21);
									cmd_inq.Fetch(tmmsmhl);
									time = tmmsm21["END_TIME"].ToDecimal() - tmmsm21["START_TIME"].ToDecimal();
									j_time = time  * actresult_avg;
									wt_sum = wt_sum + j_time;
									tmmsmhl["HEAT_NO"] = tmmsm21["HEAT_NO"];
									tmmsmhl["L2_PROC_NO"] = tmmsm21["L2_PROC_NO"];
									tmmsmhl["DEV_CODE"] = tmmsm21["DEV_CODE"];
									tmmsmhl["TOGETHER_TIME"] = j_time / 60;
									tmmsmhl["TOGETHER_TIME"] = tmmsmhl["TOGETHER_TIME"].ToDecimal().Round(0);
									tmmsmhl["PROC_NO"] = tmmsm21["PROC_NO"];
									if (tmmsmhl.Query("HEAT_NO,PROC_NO")){
										tmmsmhl.Update("TOGETHER_TIME", "HEAT_NO,PROC_NO");
									}
									else{
										tmmsmhl.Insert();
									}
								}
								cmd_inq.Close();
							}
							else if (dev_code12 == "F"){
								CDecimal j_time = 0;
								CDecimal time = 0;
								cmd_inq.SetCommandText(" SELECT * FROM TMMSM24  where HEAT_NO='" + heat_no + "' ");
								cmd_inq.ExecuteReader();
								if (cmd_inq.Read())
								{
									cmd_inq.Fetch(tmmsm24);
									cmd_inq.Fetch(tmmsmhl);
									time = tmmsm24["END_TIME"].ToDecimal() - tmmsm24["START_TIME"].ToDecimal();
									j_time = time  * actresult_avg;
									wt_sum = wt_sum + j_time;
									tmmsmhl["HEAT_NO"] = tmmsm24["HEAT_NO"];
									tmmsmhl["L2_PROC_NO"] = tmmsm24["L2_PROC_NO"];
									tmmsmhl["DEV_CODE"] = tmmsm24["DEV_CODE"];
									tmmsmhl["TOGETHER_TIME"] = j_time / 60;
									tmmsmhl["TOGETHER_TIME"] = tmmsmhl["TOGETHER_TIME"].ToDecimal().Round(0);
									tmmsmhl["PROC_NO"] = tmmsm24["PROC_NO"];
									if (tmmsmhl.Query("HEAT_NO,PROC_NO")){
										tmmsmhl.Update("TOGETHER_TIME", "HEAT_NO,PROC_NO");
									}
									else{
										tmmsmhl.Insert();
									}
								}
								cmd_inq.Close();
							}
							else if (dev_code12 == "R"){
								CDecimal j_time = 0;
								CDecimal time = 0;
								cmd_inq.SetCommandText(" SELECT * FROM TMMSM23  where HEAT_NO='" + heat_no + "' ");
								cmd_inq.ExecuteReader();
								if (cmd_inq.Read())
								{
									cmd_inq.Fetch(tmmsm23);
									cmd_inq.Fetch(tmmsmhl);
									time = tmmsm23["END_TIME"].ToDecimal() - tmmsm23["START_TIME"].ToDecimal();
									j_time = time  * actresult_avg;
									wt_sum = wt_sum + j_time;
									tmmsmhl["HEAT_NO"] = tmmsm23["HEAT_NO"];
									tmmsmhl["L2_PROC_NO"] = tmmsm23["L2_PROC_NO"];
									tmmsmhl["DEV_CODE"] = tmmsm23["DEV_CODE"];
									tmmsmhl["TOGETHER_TIME"] = j_time / 60;
									tmmsmhl["TOGETHER_TIME"] = tmmsmhl["TOGETHER_TIME"].ToDecimal().Round(0);
									tmmsmhl["PROC_NO"] = tmmsm23["PROC_NO"];
									if (tmmsmhl.Query("HEAT_NO,PROC_NO")){
										tmmsmhl.Update("TOGETHER_TIME", "HEAT_NO,PROC_NO");
									}
									else{
										tmmsmhl.Insert();
									}
								}
								cmd_inq.Close();
							}
							else if (dev_code12 == "S"){
								CDecimal j_time = 0;
								CDecimal time = 0;
								cmd_inq.SetCommandText(" SELECT * FROM TMMSM26  where HEAT_NO='" + heat_no + "' ");
								cmd_inq.ExecuteReader();
								if (cmd_inq.Read())
								{
									cmd_inq.Fetch(tmmsm26);
									cmd_inq.Fetch(tmmsmhl);
									time = tmmsm26["END_TIME"].ToDecimal() - tmmsm26["START_TIME"].ToDecimal();
									j_time = time  * actresult_avg;
									wt_sum = wt_sum + j_time;
									tmmsmhl["HEAT_NO"] = tmmsm26["HEAT_NO"];
									tmmsmhl["L2_PROC_NO"] = tmmsm26["L2_PROC_NO"];
									tmmsmhl["DEV_CODE"] = tmmsm26["DEV_CODE"];
									tmmsmhl["TOGETHER_TIME"] = j_time / 60;
									tmmsmhl["TOGETHER_TIME"] = tmmsmhl["TOGETHER_TIME"].ToDecimal().Round(0);
									tmmsmhl["PROC_NO"] = tmmsm26["PROC_NO"];
									if (tmmsmhl.Query("HEAT_NO,PROC_NO")){
										tmmsmhl.Update("TOGETHER_TIME", "HEAT_NO,PROC_NO");
									}
									else{
										tmmsmhl.Insert();
									}
								}
								cmd_inq.Close();
							}
							else if (dev_code12 == "V")
							{
								CDecimal j_time = 0;
								CDecimal time = 0;
								cmd_inq.SetCommandText(" SELECT * FROM TMMSM25  where HEAT_NO='" + heat_no + "' ");
								cmd_inq.ExecuteReader();
								if (cmd_inq.Read())
								{
									cmd_inq.Fetch(tmmsm25);
									cmd_inq.Fetch(tmmsmhl);
									time = tmmsm25["END_TIME"].ToDecimal() - tmmsm25["START_TIME"].ToDecimal();
									j_time = time  * actresult_avg;
									wt_sum = wt_sum + j_time;
									tmmsmhl["HEAT_NO"] = tmmsm25["HEAT_NO"];
									tmmsmhl["L2_PROC_NO"] = tmmsm25["L2_PROC_NO"];
									tmmsmhl["DEV_CODE"] = tmmsm25["DEV_CODE"];
									tmmsmhl["TOGETHER_TIME"] = j_time / 60;
									tmmsmhl["TOGETHER_TIME"] = tmmsmhl["TOGETHER_TIME"].ToDecimal().Round(0);
									tmmsmhl["PROC_NO"] = tmmsm25["PROC_NO"];
									if (tmmsmhl.Query("HEAT_NO,PROC_NO")){
										tmmsmhl.Update("TOGETHER_TIME", "HEAT_NO,PROC_NO");
									}
									else{
										tmmsmhl.Insert();
									}
								}
								cmd_inq.Close();

							}
						}
						cmd_inq12.Close();
					}
				}
			}
		}
		else if (flag == "2"){//工艺路线
			heat_no = bcls_rec->Tables[0].Rows[0]["HEAT_NO"].ToString().Trim();
			l2_proc_no = bcls_rec->Tables[0].Rows[0]["L2_PROC_NO"].ToString().Trim();
			proc_no = bcls_rec->Tables[0].Rows[0]["PROC_NO"].ToString().Trim();
			CDecimal together_time = 0;
			cmd_inqhl.SetCommandText(" select TOGETHER_TIME from TMMSMHL WHERE HEAT_NO='" + heat_no + "' and PROC_NO='" + proc_no + "' ");
			cmd_inqhl.ExecuteReader();
			if (cmd_inqhl.Read())
			{
				together_time = cmd_inqhl.GetDecimal(1);
			}
			cmd_inqhl.Close();
			cmd_inqgylx.SetCommandText(" select * from TMMSMGY06 WHERE HEAT_NO='" + heat_no + "' and PROC_NO='" + proc_no + "' ");
			cmd_inqgylx.ExecuteReader();
			if (cmd_inqgylx.Read())
			{
				cmd_inqgylx.Fetch(tmmsmgy06);
				//判断是否已发送电文，没发送则不需要给
				if (tmmsmgy06["TC_SEND_FLAG"].ToString() != "2"){
					tmmsmgy06["DURATION_TIME"] = tmmsmgy06["DURATION_TIME"].ToDecimal() - together_time;
					tmmsmgy06["DURATION_TIME"] = tmmsmgy06["DURATION_TIME"].ToDecimal().Round(0);
					tmmsmgy06.Update("DURATION_TIME", "PROC_NO,HEAT_NO");
				}
				else{
					tmmsmgy06["TOGETHER_TIME"] = tmmsmgy06["DURATION_TIME"].ToString();
					tmmsmgy06.Update("DURATION_TIME,TOGETHER_TIME", "PROC_NO,HEAT_NO");
				}
			}
			cmd_inqgylx.Close();
		}

		//if (wt != 0 && m_heat_no!=" "){
		//	根据传过来的工位判断上一工序是哪个表
		//	if (dev_code == "Z"){
		//		table_name = "TMMSM19";
		//	}
		//	else if (dev_code == "A"){
		//		table_name = "TMMSM27";
		//	}
		//	else if (dev_code == "E"){
		//		table_name = "TMMSM20";
		//	}
		//	else if (dev_code == "B"){
		//		table_name = "TMMSM21";
		//	}
		//	/*else if (dev_code == "F"){
		//	table_name = "TMMSM24";
		//	}*/
		//	else if (dev_code == "R"){
		//		table_name = "TMMSM23";
		//	}
		//	/*else if (dev_code == "S"){
		//	table_name = "tmmsm26";
		//	}*/
		//	else if (dev_code == "V")
		//	{
		//		table_name = "tmmsm25";
		//	}
		//	Log::Trace("", __FUNCTION__, "table_name=[{0}]", table_name);
		//	根据表去查该工序的实绩重量
		//	cmd_sql.SetCommandText(" SELECT ACTRESULT FROM " + table_name + "  where HEAT_NO='" + heat_no + "' ");
		//	cmd_sql.ExecuteReader();
		//	if (cmd_sql.Read())
		//	{
		//		actresult = cmd_sql.GetDecimal(1);
		//	}
		//	cmd_sql.Close();
		//	Log::Trace("", __FUNCTION__, "actresult=[{0}]", actresult);
		//	if (actresult != 0){
		//		根据计划传过来的重量/实绩重量=每工序要减去的值
		//		actresult_avg = wt / actresult;
		//		Log::Trace("", __FUNCTION__, "actresult_avg=[{0}]", actresult_avg);
		//		获取该熔炼号有多少导工序
		//		cmd_inq12.SetCommandText(" select DEV_CODE from TPSSM12 WHERE HEAT_NO='" + heat_no + "' ");
		//		cmd_inq12.ExecuteReader();
		//		while (cmd_inq12.Read())
		//		{
		//			dev_code12 = cmd_inq12.GetString(1).Substring(0, 1);
		//			Log::Trace("", __FUNCTION__, "dev_code12=[{0}]", dev_code12);
		//			if (dev_code12 == "Z"){
		//				根据表去查该工序的实绩重量
		//				CDecimal j_time = 0;
		//				CDecimal time = 0;
		//				cmd_inq.SetCommandText(" SELECT * FROM TMMSM19  where HEAT_NO='" + heat_no + "' ");
		//				cmd_inq.ExecuteReader();
		//				if (cmd_inq.Read())
		//				{
		//					cmd_inq.Fetch(tmmsm19);
		//					cmd_inq.Fetch(tmmsmgy06);
		//					time = tmmsm19["END_TIME"].ToDecimal() - tmmsm19["START_TIME"].ToDecimal();
		//					j_time = time  * actresult_avg;
		//					wt_sum = wt_sum + j_time;
		//					tmmsmgy06["HEAT_NO"] = tmmsm19["HEAT_NO"];
		//					tmmsmgy06["L2_PROC_NO"] = tmmsm19["L2_PROC_NO"];
		//					tmmsmgy06["DEV_CODE"] = tmmsm19["DEV_CODE"];
		//					tmmsmgy06["TOGETHER_TIME"] = j_time;
		//					if (tmmsmgy06.Query("HEAT_NO,L2_PROC_NO,DEV_CODE")){
		//						tmmsmgy06["DURATION_TIME"] = time - j_time;
		//						tmmsmgy06.Update("DURATION_TIME,TOGETHER_TIME", "HEAT_NO,L2_PROC_NO,DEV_CODE");
		//					}
		//					else{
		//						tmmsmgy06["TOGETHER_TIME"] = j_time;
		//						tmmsmgy06.Insert();
		//					}
		//				}
		//				cmd_inq.Close();
		//			}
		//			else if (dev_code12 == "A"){
		//				根据表去查该工序的实绩重量
		//				CDecimal j_time = 0;
		//				CDecimal time = 0;
		//				cmd_inq.SetCommandText(" SELECT * FROM TMMSM27  where HEAT_NO='" + heat_no + "' ");
		//				cmd_inq.ExecuteReader();
		//				if (cmd_inq.Read())
		//				{
		//					cmd_inq.Fetch(tmmsm27);
		//					cmd_inq.Fetch(tmmsmgy06);
		//					time = tmmsm27["END_TIME"].ToDecimal() - tmmsm27["START_TIME"].ToDecimal();
		//					j_time = time  * actresult_avg;
		//					wt_sum = wt_sum + j_time;
		//					tmmsmgy06["HEAT_NO"] = tmmsm27["HEAT_NO"];
		//					tmmsmgy06["L2_PROC_NO"] = tmmsm27["L2_PROC_NO"];
		//					tmmsmgy06["DEV_CODE"] = tmmsm27["DEV_CODE"];
		//					tmmsmgy06["TOGETHER_TIME"] = j_time;
		//					if (tmmsmgy06.Query("HEAT_NO,L2_PROC_NO,DEV_CODE")){
		//						tmmsmgy06["DURATION_TIME"] = time - j_time;
		//						tmmsmgy06.Update("DURATION_TIME,TOGETHER_TIME", "HEAT_NO,L2_PROC_NO,DEV_CODE");
		//					}
		//					else{
		//						tmmsmgy06["TOGETHER_TIME"] = j_time;
		//						tmmsmgy06.Insert();
		//					}
		//				}
		//				cmd_inq.Close();
		//			}
		//			else if (dev_code12 == "E"){
		//				CDecimal j_time = 0;
		//				CDecimal time = 0;
		//				cmd_inq.SetCommandText(" SELECT * FROM TMMSM20  where HEAT_NO='" + heat_no + "' ");
		//				cmd_inq.ExecuteReader();
		//				if (cmd_inq.Read())
		//				{
		//					cmd_inq.Fetch(tmmsm20);
		//					cmd_inq.Fetch(tmmsmgy06);
		//					time = tmmsm20["END_TIME"].ToDecimal() - tmmsm20["START_TIME"].ToDecimal();
		//					j_time = time  * actresult_avg;
		//					wt_sum = wt_sum + j_time;
		//					tmmsmgy06["HEAT_NO"] = tmmsm20["HEAT_NO"];
		//					tmmsmgy06["L2_PROC_NO"] = tmmsm20["L2_PROC_NO"];
		//					tmmsmgy06["DEV_CODE"] = tmmsm20["DEV_CODE"];
		//					tmmsmgy06["TOGETHER_TIME"] = j_time;
		//					if (tmmsmgy06.Query("HEAT_NO,L2_PROC_NO,DEV_CODE")){
		//						tmmsmgy06["DURATION_TIME"] = time - j_time;
		//						tmmsmgy06.Update("DURATION_TIME,TOGETHER_TIME", "HEAT_NO,L2_PROC_NO,DEV_CODE");
		//					}
		//					else{
		//						tmmsmgy06["TOGETHER_TIME"] = j_time;
		//						tmmsmgy06.Insert();
		//					}
		//				}
		//				cmd_inq.Close();
		//			}
		//			else if (dev_code12 == "B"){
		//				CDecimal j_time = 0;
		//				CDecimal time = 0;
		//				cmd_inq.SetCommandText(" SELECT * FROM TMMSM21  where HEAT_NO='" + heat_no + "' ");
		//				cmd_inq.ExecuteReader();
		//				if (cmd_inq.Read())
		//				{
		//					cmd_inq.Fetch(tmmsm21);
		//					cmd_inq.Fetch(tmmsmgy06);
		//					time = tmmsm21["END_TIME"].ToDecimal() - tmmsm21["START_TIME"].ToDecimal();
		//					j_time = time  * actresult_avg;
		//					wt_sum = wt_sum + j_time;
		//					tmmsmgy06["HEAT_NO"] = tmmsm21["HEAT_NO"];
		//					tmmsmgy06["L2_PROC_NO"] = tmmsm21["L2_PROC_NO"];
		//					tmmsmgy06["DEV_CODE"] = tmmsm21["DEV_CODE"];
		//					tmmsmgy06["TOGETHER_TIME"] = j_time;
		//					if (tmmsmgy06.Query("HEAT_NO,L2_PROC_NO,DEV_CODE")){
		//						tmmsmgy06["DURATION_TIME"] = time - j_time;
		//						tmmsmgy06.Update("DURATION_TIME,TOGETHER_TIME", "HEAT_NO,L2_PROC_NO,DEV_CODE");
		//					}
		//					else{
		//						tmmsmgy06["TOGETHER_TIME"] = j_time;
		//						tmmsmgy06.Insert();
		//					}
		//				}
		//				cmd_inq.Close();
		//			}
		//			else if (dev_code12 == "F"){
		//				CDecimal j_time = 0;
		//				CDecimal time = 0;
		//				cmd_inq.SetCommandText(" SELECT * FROM TMMSM24  where HEAT_NO='" + heat_no + "' ");
		//				cmd_inq.ExecuteReader();
		//				if (cmd_inq.Read())
		//				{
		//					cmd_inq.Fetch(tmmsm24);
		//					cmd_inq.Fetch(tmmsmgy06);
		//					time = tmmsm24["END_TIME"].ToDecimal() - tmmsm24["START_TIME"].ToDecimal();
		//					j_time = time  * actresult_avg;
		//					wt_sum = wt_sum + j_time;
		//					tmmsmgy06["HEAT_NO"] = tmmsm24["HEAT_NO"];
		//					tmmsmgy06["L2_PROC_NO"] = tmmsm24["L2_PROC_NO"];
		//					tmmsmgy06["DEV_CODE"] = tmmsm24["DEV_CODE"];
		//					tmmsmgy06["TOGETHER_TIME"] = j_time;
		//					if (tmmsmgy06.Query("HEAT_NO,L2_PROC_NO,DEV_CODE")){
		//						tmmsmgy06["DURATION_TIME"] = time - j_time;
		//						tmmsmgy06.Update("DURATION_TIME,TOGETHER_TIME", "HEAT_NO,L2_PROC_NO,DEV_CODE");
		//					}
		//					else{
		//						tmmsmgy06["TOGETHER_TIME"] = j_time;
		//						tmmsmgy06.Insert();
		//					}
		//				}
		//				cmd_inq.Close();
		//			}
		//			else if (dev_code12 == "R"){
		//				CDecimal j_time = 0;
		//				CDecimal time = 0;
		//				cmd_inq.SetCommandText(" SELECT * FROM TMMSM23  where HEAT_NO='" + heat_no + "' ");
		//				cmd_inq.ExecuteReader();
		//				if (cmd_inq.Read())
		//				{
		//					cmd_inq.Fetch(tmmsm23);
		//					cmd_inq.Fetch(tmmsmgy06);
		//					time = tmmsm23["END_TIME"].ToDecimal() - tmmsm23["START_TIME"].ToDecimal();
		//					j_time = time  * actresult_avg;
		//					wt_sum = wt_sum + j_time;
		//					tmmsmgy06["HEAT_NO"] = tmmsm23["HEAT_NO"];
		//					tmmsmgy06["L2_PROC_NO"] = tmmsm23["L2_PROC_NO"];
		//					tmmsmgy06["DEV_CODE"] = tmmsm23["DEV_CODE"];
		//					tmmsmgy06["TOGETHER_TIME"] = j_time;
		//					if (tmmsmgy06.Query("HEAT_NO,L2_PROC_NO,DEV_CODE")){
		//						tmmsmgy06["DURATION_TIME"] = time - j_time;
		//						tmmsmgy06.Update("DURATION_TIME,TOGETHER_TIME", "HEAT_NO,L2_PROC_NO,DEV_CODE");
		//					}
		//					else{
		//						tmmsmgy06["TOGETHER_TIME"] = j_time;
		//						tmmsmgy06.Insert();
		//					}
		//				}
		//				cmd_inq.Close();
		//			}
		//			else if (dev_code12 == "S"){
		//				CDecimal j_time = 0;
		//				CDecimal time = 0;
		//				cmd_inq.SetCommandText(" SELECT * FROM TMMSM26  where HEAT_NO='" + heat_no + "' ");
		//				cmd_inq.ExecuteReader();
		//				if (cmd_inq.Read())
		//				{
		//					cmd_inq.Fetch(tmmsm26);
		//					cmd_inq.Fetch(tmmsmgy06);
		//					time = tmmsm26["END_TIME"].ToDecimal() - tmmsm26["START_TIME"].ToDecimal();
		//					j_time = time  * actresult_avg;
		//					wt_sum = wt_sum + j_time;
		//					tmmsmgy06["HEAT_NO"] = tmmsm26["HEAT_NO"];
		//					tmmsmgy06["L2_PROC_NO"] = tmmsm26["L2_PROC_NO"];
		//					tmmsmgy06["DEV_CODE"] = tmmsm26["DEV_CODE"];
		//					tmmsmgy06["TOGETHER_TIME"] = j_time;
		//					if (tmmsmgy06.Query("HEAT_NO,L2_PROC_NO,DEV_CODE")){
		//						tmmsmgy06["DURATION_TIME"] = time - j_time;
		//						tmmsmgy06.Update("DURATION_TIME,TOGETHER_TIME", "HEAT_NO,L2_PROC_NO,DEV_CODE");
		//					}
		//					else{
		//						tmmsmgy06["TOGETHER_TIME"] = j_time;
		//						tmmsmgy06.Insert();
		//					}
		//				}
		//				cmd_inq.Close();
		//			}
		//			else if (dev_code12 == "V")
		//			{
		//				CDecimal j_time = 0;
		//				CDecimal time = 0;
		//				cmd_inq.SetCommandText(" SELECT * FROM TMMSM25  where HEAT_NO='" + heat_no + "' ");
		//				cmd_inq.ExecuteReader();
		//				if (cmd_inq.Read())
		//				{
		//					cmd_inq.Fetch(tmmsm25);
		//					cmd_inq.Fetch(tmmsmgy06);
		//					time = tmmsm25["END_TIME"].ToDecimal() - tmmsm25["START_TIME"].ToDecimal();
		//					j_time = time  * actresult_avg;
		//					wt_sum = wt_sum + j_time;
		//					tmmsmgy06["HEAT_NO"] = tmmsm25["HEAT_NO"];
		//					tmmsmgy06["L2_PROC_NO"] = tmmsm25["L2_PROC_NO"];
		//					tmmsmgy06["DEV_CODE"] = tmmsm25["DEV_CODE"];
		//					tmmsmgy06["TOGETHER_TIME"] = j_time;
		//					if (tmmsmgy06.Query("HEAT_NO,L2_PROC_NO,DEV_CODE")){
		//						tmmsmgy06["DURATION_TIME"] = time - j_time;
		//						tmmsmgy06.Update("DURATION_TIME,TOGETHER_TIME,TOGETHER_TIME", "HEAT_NO,L2_PROC_NO,DEV_CODE");
		//					}
		//					else{
		//						tmmsmgy06["TOGETHER_TIME"] = j_time;
		//						tmmsmgy06.Insert();
		//					}
		//				}
		//				cmd_inq.Close();
		//			}
		//		}
		//		cmd_inq12.Close();
				////根据要添加的熔炼号去加重量
				//cmd_inqj.SetCommandText(" select DEV_CODE from TPSSM12 WHERE HEAT_NO='" + m_heat_no + "' ");
				////获取条数
				//CDecimal count = cmd_inqj.ExecuteScalar();
				////获取每工序加多少重量
				//CDecimal avg = wt_sum / count;
				//cmd_inqj.ExecuteReader();
				//while (cmd_inqj.Read())
				//{
				//	CString dev_codex = cmd_inq12.GetString(1).Substring(0, 1);
				//	if (dev_codex == "Z"){
				//		//根据表去查该工序的实绩重量
				//		cmd_inq.SetCommandText(" SELECT * FROM TMMSM19  where HEAT_NO='" + m_heat_no + "' ");
				//		cmd_inq.ExecuteReader();
				//		if (cmd_inq.Read())
				//		{
				//			cmd_inq.Fetch(tmmsm19);
				//			tmmsm19["ACTRESULT"] = tmmsm19["ACTRESULT"].ToDecimal() + avg;
				//			tmmsm19.Update("ACTRESULT", "HEAT_NO");
				//		}
				//		cmd_inq.Close();
				//	}
				//	else if (dev_codex == "A"){
				//		cmd_inq.SetCommandText(" SELECT * FROM TMMSM27  where HEAT_NO='" + m_heat_no + "' ");
				//		cmd_inq.ExecuteReader();
				//		if (cmd_inq.Read())
				//		{
				//			cmd_inq.Fetch(tmmsm27);
				//			tmmsm27["ACTRESULT"] = tmmsm27["ACTRESULT"].ToDecimal() + avg;
				//			tmmsm27.Update("ACTRESULT", "HEAT_NO");
				//		}
				//		cmd_inq.Close();
				//	}
				//	else if (dev_codex == "E"){
				//		cmd_inq.SetCommandText(" SELECT * FROM TMMSM20  where HEAT_NO='" + m_heat_no + "' ");
				//		cmd_inq.ExecuteReader();
				//		if (cmd_inq.Read())
				//		{
				//			cmd_inq.Fetch(tmmsm20);
				//			tmmsm20["ACTRESULT"] = tmmsm20["ACTRESULT"].ToDecimal() + avg;
				//			tmmsm20.Update("ACTRESULT", "HEAT_NO");
				//		}
				//		cmd_inq.Close();
				//	}
				//	else if (dev_codex == "B"){
				//		cmd_inq.SetCommandText(" SELECT * FROM TMMSM21  where HEAT_NO='" + m_heat_no + "' ");
				//		cmd_inq.ExecuteReader();
				//		if (cmd_inq.Read())
				//		{
				//			cmd_inq.Fetch(tmmsm21);
				//			tmmsm21["ACTRESULT"] = tmmsm21["ACTRESULT"].ToDecimal() + avg;
				//			tmmsm21.Update("ACTRESULT", "HEAT_NO");
				//		}
				//		cmd_inq.Close();
				//		
				//	}
				//	else if (dev_codex == "F"){
				//		cmd_inq.SetCommandText(" SELECT * FROM TMMSM24  where HEAT_NO='" + m_heat_no + "' ");
				//		cmd_inq.ExecuteReader();
				//		if (cmd_inq.Read())
				//		{
				//			cmd_inq.Fetch(tmmsm24);
				//			tmmsm24["ACTRESULT"] = tmmsm24["ACTRESULT"].ToDecimal() + avg;
				//			tmmsm24.Update("ACTRESULT", "HEAT_NO");
				//		}
				//		cmd_inq.Close();
				//	}
				//	else if (dev_codex == "R"){
				//		/*cmd_inq.SetCommandText(" SELECT * FROM TMMSM23  where HEAT_NO='" + m_heat_no + "' ");
				//		cmd_inq.ExecuteReader();
				//		if (cmd_inq.Read())
				//		{
				//			cmd_inq.Fetch(tmmsm23);
				//			tmmsm23["ACTRESULT"] = tmmsm23["ACTRESULT"].ToDecimal() + avg;
				//			tmmsm23.Update("ACTRESULT", "HEAT_NO");
				//		}
				//		cmd_inq.Close();*/
				//	}
				//	else if (dev_codex == "S"){
				//		/*cmd_inq.SetCommandText(" SELECT * FROM TMMSM26  where HEAT_NO='" + m_heat_no + "' ");
				//		cmd_inq.ExecuteReader();
				//		if (cmd_inq.Read())
				//		{
				//			cmd_inq.Fetch(tmmsm24);
				//			tmmsm24["ACTRESULT"] = tmmsm24["ACTRESULT"].ToDecimal() + avg;
				//			tmmsm24.Update("ACTRESULT", "HEAT_NO");
				//		}
				//		cmd_inq.Close();*/
				//	}
				//	else if (dev_codex == "V")
				//	{
				//		cmd_inq.SetCommandText(" SELECT * FROM TMMSM25  where HEAT_NO='" + m_heat_no + "' ");
				//		cmd_inq.ExecuteReader();
				//		if (cmd_inq.Read())
				//		{
				//			cmd_inq.Fetch(tmmsm25);
				//			tmmsm25["ACTRESULT"] = tmmsm25["ACTRESULT"].ToDecimal() + avg;
				//			tmmsm25.Update("ACTRESULT", "HEAT_NO");
				//		}
				//		cmd_inq.Close();
				//	}
				//}
			/*	cmd_inqj.Close();
			}
		}*/
		
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


