
#include "stdafx.h"


BM2_FUNCTION_EXPORT


int f_mmsm_unitwt(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	int doFlag = 0;
	CString sqlstr = " ";
	CString slabType = "";
	CString operatorType = ""; //小代码MS42
	CDecimal slabThick = 0;
	CDecimal slabWidth = 0;
	CDecimal slabLen = 0;
	CDecimal wtPerMeter = 0;
	CDecimal totalNetWt = 0;
	CDecimal totalTube = 0;
	CDecimal i_prod_density = 7.85; //密度
	CString isInTable = "";
	CString ingotCode = "";
	CString sgSign = "";
	int blkNum = 0;

	CModel tmmsmwt("TMMSMWT");
	CModel tmmsmwta("TMMSMWTA");
	try
	{
		/*blkNum = bcls_rec->Tables.IndexOf("PARA");
		if (blkNum < 0)
		{
			bcls_rec->Tables.Add("PARA");
		}*/
		if(bcls_rec->Tables[0].Columns.Contains("OPERATOR_TYPE"))
		{
			operatorType = bcls_rec->Tables[0].Rows[0]["OPERATOR_TYPE"];
		}
		//Log::Trace("", "", "operatorType=[{0}]", operatorType);
		slabType = bcls_rec->Tables[0].Rows[0]["SLAB_TYPE"];
		//Log::Trace("", "", "slabType=[{0}]", slabType);
		tmmsmwt.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		tmmsmwta.MergeFrom(bcls_rec->Tables[0].Rows[0]);
		CString ingotCode = tmmsmwt["INGOT_CODE"];
		CString sgSign = tmmsmwt["SG_SIGN"];

		//计算单位重量start
		if (tmmsmwta["SLAB_THICK"].ToDecimal() >0){
			slabThick = tmmsmwta["SLAB_THICK"];
		}
		if (tmmsmwta["SLAB_WIDTH"].ToDecimal() >0){
			slabWidth = tmmsmwta["SLAB_WIDTH"];
		}
		if (tmmsmwta["SLAB_LEN"].ToDecimal() >0){
			slabLen = tmmsmwta["SLAB_LEN"];
		}
		if (tmmsmwta["TOTAL_NET_WT"].ToDecimal() >0){
			totalNetWt = tmmsmwta["TOTAL_NET_WT"];
		}
		if (tmmsmwta["TOTAL_TUBE"].ToDecimal() >0){
			totalTube = tmmsmwta["TOTAL_TUBE"];
		}
		if (tmmsmwta["WT_PER_METER"].ToDecimal() >0){
			wtPerMeter = tmmsmwta["WT_PER_METER"];
		}
		if (operatorType != "0" && slabType != "5"){
			if ((slabThick>0 && slabWidth>0 && operatorType == "2") || (slabLen>0 && totalNetWt>0 && totalTube>0 && operatorType == "1")){
				if (operatorType == "2" && (slabType == "3" || slabType == "4")){ //操作标记为1密度计算
					if (slabType == "3"){ //方坯
						wtPerMeter = ((slabThick / 1000) * (slabWidth / 1000) * i_prod_density).Round(3);
					}
					else if (slabType == "4"){ //圆坯
						wtPerMeter = ((slabThick / 2000) * (slabThick / 2000) * 3.14 *  i_prod_density).Round(3);
					}
				}
				else if (operatorType == "1" && (slabType == "3" || slabType == "4")){//操作标记为2规格计算
					wtPerMeter = (totalNetWt / totalTube / (slabLen / 1000)).Round(3);
				}
				else{//5铸锭或其他 操作标记为0
					//直接取单位重量
				}
				//Log::Trace("", "", "路径=计算");
			}
			else{
				wtPerMeter = ((slabThick / 1000) * (slabWidth / 1000) * i_prod_density).Round(3);
			}
			tmmsmwta["WT_PER_METER"] = wtPerMeter; 
			tmmsmwta["SLAB_THICK"] = slabThick;
			tmmsmwta["SLAB_WIDTH"] = slabWidth;
			tmmsmwta["SLAB_LEN"] = slabLen;
			/*tmmsmwta["TOTAL_NET_WT"] = totalNetWt;
			tmmsmwta["TOTAL_TUBE"] = totalTube*/;
		}
		
		//计算单位重量end
		//Log::Trace("", "", "slabThick=[{0}] slabWidth=[{1}] slabLen=[{2}] wtPerMeter=[{3}] sgSign=[{4}] totalNetWt=[{5}] totalTube=[{6}]", slabThick, slabWidth, slabLen, wtPerMeter, sgSign, totalNetWt, totalTube);
		//tmmsmwt.Print();
		tmmsmwt.CopyFrom(tmmsmwta);
		tmmsmwt.MergeTo(bcls_ret->Tables[0], false);
		// -----Begin IPLAT4C::IPLAT4CServiceCompositeStatementObj()----- //
		// -----End IPLAT4C::IPLAT4CServiceCompositeStatementObj()----- //
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


