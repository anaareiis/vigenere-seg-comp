#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

#include "../estimativa_tamanho_chave.h"

using namespace std;

// ============================================================================
// ESTRUTURA DE TESTES
// ============================================================================
struct CasoDeTeste {
    string nome;
    string texto_cifrado;
    int tamanho_real_chave;
};

int main() {
    const int MAX_TAMANHO = 20;

    // Adicione seus textos cifrados aqui. 
    vector<CasoDeTeste> testes = {
        {
            "Teste 1 - #'encrypt' (7)", 
            "Xug hsxvo, otfuc ysk llkel sigi y atdl ffe. SCw snfaz uc jjvl BMZ nz hsxs teqx. Hjgo ZVM ojbd ttratw fl hfv laiyrj. Zpphf lfe, uempm hspkxm, xvv crqcjj. Upexm, drb crqcj, wmg jyveb hxzw igo! Ddq rlogfh zvnd hsxvo-wkmcs peyvq. Zgbgx slgo plnpxq ynqca mcawx sqo. Zgbkuv mgmxrf llke; wsma wmle uhcti. Fnmpm nyummai qceaceu mcm usyf Agb. Jyveb xtilltj zaha, igogcz hnhk Hxf. Wrz-tfpkkrf wme upry dw ynrx VM ojbd. Uqn ojbgxnp bpyx wwdnxgk mgsppl zrz. Kud wvvxvl yhgxu ycai jnz dw qbk dwzx. Fnmpm, Syo, zig op udoia hcym csqjgsgl! \"Rby wym jyvb Ayrd!\" ql diykx kuqjr eeiq. Hztt jynebgcz drrywgl nbnk kn pek dvb. Ueyzofvtw fl lfz, zoigeygcz A. mcgq Xked. Efxn ltukev ltzru hspkx wwx mu ueq ozjz. T zrtp zpw uhcti bbkuv agcq dvrgw uhayu. Wcl jyvrj epeznpzxtw xug dmrd nhtp zdq. Uhkti qksjp umvl nhog mkxv gjv jpsc sqo. Rwx nna, ggv, ysk, bvzgt, eaf dw lhpigj ojtgx! Dcmlsc egu txqiau wgvax sqi y fnmpm asbi. Nbchsxg Tuqvlxq anu xyoxh oa DRK yse nlaz. T avbrps’l nbd zq ih zrz tfjftf slgrdpl ke ddz. Anvtf \"Yxsccibn!\", Tprz Kptuix'u wsc MZ dwzx vtqr. Yfttg wvnb nncezcj cmvlnpxcs yse dcst jyntkx. Qkejpp edww wwjr uespmvb ji xb slgo trq xvv wbq. Nfashmmai hsxoie ceb qha, Mqdnnv[1] ovnccs mlr hfv. Br jnzvb yhor yfl p ietgi gc mlr erzax XI slgo llby. Rkpsmaicw uxa qkjadmlrslch ivbxzbt cyxgsmmxw. Za xgge abxv qxq hbbvl eeevf ayrdigu scuhvr uyc fnmg. Uzv qbk qgmgal jeqd Hpiea slgrdpl hfpvhx uqn rd peyvq. Zxz Nhnp cpkxuslyzxw pqeddnrq brln xbcgigbxrgcc tdp. Jbzp npkwbpj ojbd npu apcsyg kft esicsjn wmz yzix-zmen. Yykx e ckti: ipiavp qxq prvkcgl - rb hfprbrt c asbuprf hsxs! Gecqw Ukiqgigrde oqlewm qnpp ttkc rzhsxlmgg fnpe nryvjh. Lmkvp xxitrtj utki dwzazec cktitw jeqd rwx abxvl ynxr dre. P jyveb kdoizger dy xug vltfc jkcj yxsccibxsi fko ejgfbckq. Pep dwvqibsau rqzxh oa wgkx anvtf tqtrtkq pfemgu rwx nhfxc. Ytgx slgtmpl ofttw yc himcm eaf jcxsiq vyc qbk occj dy anz.Kft jyveb, zghaa hfv ynqcu fttk e ycqw shk. QLj dahgx dp uwxr ZVM ym jyvb gpdz. Nhpb KIO uhkq egtgrf sw uhb jjvjel. Fnyuq yhk, snzaz jyntkx, kxb aadnwl. Annkx, qth aadnw, yse slgrd nvij ttq! Jbz ewbilf iiyq jyveb-hxoiq yrjis. Fekti fnmm yyyczw wwdnn oiyfk ddq. Fekxfi omkgeq ynqc; ffxn ysjn hspvo. Dwzaz pesvzlv sicjpph oik dfjs Cmz. Slgrd drrywgl fyqn, ttqmai uyum Nvo",
            7
        },
        {
            "Teste 2 - #'grandechave' (11)", 
            "Zye Rxvqwevr rrntxeils vvk dezeitz oa xnv snpi hhmdpe. Khrlv ulpvvgke ramuaeigk zs n pcvo. Fjv ytirqgg, tunmi, jpbux, gac, Zyxfpr xwgz tci yrmr ysehbppgiy. Gki nhnbygxef rrnf ddjlvr vq xjlim kxrmzdv, voedv vioaxrepaomue aag xjlim qujt prqovn rsxus. Ryitfoii xvayldgz wcc g eej fsotoi pgeghdkg doppj se qhwkyawpk: fnr fswsd millsr ws rhy zbvvnflzg arvrycagrvu. Ao vgnzeih xjps, dx cfuyg fg uexiyjaeb xq oaqi aeisrvo nrvqsrr, cuspbnxmgkibq epk mjvk tozpsp domhy. Zf fhzgyag pgeghdkgz cjervsph, xjl gmesdae rj voe miyllglri saikargr lw ovrz wodpyh epk rzkacae wlcu tcez ff gki kuddzouuno pcugpemvs. Gki plw xssdoa oepnuvkk niyo fg tomi yzmcoi cud mimllnu xjhn olk vxvvxkug Zyxfprdr nhnbygxef. Lx yplg fk rs flqrse vw Utcvgipaag; mt wapw, mv digp hv Opfmflnoer. Ko nq Ipnldwn geevsp, pt rmrc srhq npkz wodpyljkld Zrmcifk, eu h sfivkipdp Ehmwvougr ivklny sl diah xqsd hi cyag Rgepdzrzrl vv.Xjl Epvugenq pcugpemvs nui olmwixj os wlg zahi lrmvoc. Voedv yvpnuevl esmykeafi kz a hczy. Fbu wepeigk, duflg, uwomx, kkc, Rxvqwe pwkj tuh wcte qsirbhoetf. Tci rrntxeils jrrp dvijgy ii xnvie jvctmvv, zyevu ttvnprizaglsp hny xnvie psua cjqsfn jrvfz. Eqixpoah vghlddkj wub e plw xssdoa oepnuvkk nohoh dl dzwoiaooi: que xsacd ehjwze os vry ratgusdzk krnqwnhtjvy.", 
            11
        },
        {
            "Teste 3 - #'chave' (5)", 
            "Hhr aet hwvc, dlhdrf ahz aqyd hswutvmpz, fvv hyoh xjl cjypardiu Cofenpa vrf Joiwquaixkh, tcitl ldzg ahz fnpny xgetn. Wgwamevld olgf ldzg pn Wsqrmvvmzgmsxl rdkja ao xjl cjeua oa xjl Szqcutdgu, h lvvil lvribabi qjevr. C zmvpn yiqit uahif Kuyip mljau iy olgpr kpcje vrf zuktnpen mv diol voe iielsnetf rzkgsivpkh. Io mu h pvvckiniohtdg evuixtf, ii ajpcc vqhsoif wamxu vf nipaeiggz fgc kutj cqbr hswah. Zzgu tci csl-ksylrayn Wodrvpnb lcz nj gqutmsn hbjyv ahz fnpny xgetn mv ps vr csmjwv bnjvvoobvcwhdg npfz Spl dvc jvwzzgy a nqcsl gmpl oa fnpny xget wc voe ieol oa Pqyeh Mrzuh hgjiyif ao gicce ast ahz jcy Wjvnk oa Kthmhet. Ahz Fkn Osqqe ayzkzey lgy njx vv dj wq, iexewze olgye ritl tcswzaihu vf wef Johqcz, wdpf Xuzwvpoi Qcykn epk dzzkvun Wgtifsnp, bpx voe Gmvalz Fnpny Xget ymfu’t gmuaei. Wjl pvgmld cit zeqip cemwcsiv, twa hzv kuiomcs iixq ahz fgst vrf tayi jlrninm oi xjl wvc. Yoei wjl rzeeoey xjl fdvua hdpnz oa xjl Ioenpc Hswutvmpz, sci jhd v pczt qmgd bvgm vn olg zktpkue jj jlr csoltjap Iojoohrfwiyoqi, voe cickldrg vf Vproawiv Cigpcne vrf ahz wwildrg vf cit vwi vqhd, olg Siii Nhnz. Tkayayn h rzxjvrdg sbenxkvn mep vvzv jlr xlglk, olgu",
            5
        },
        {
            "Teste 4 - #'chavemuitogrande' (16)", 
            "Que hsdhqgu, cyea Jvgnom Wmgat kube suso arjynfmw rxvazv, lg moprp bqfgkcf guepzfjvyyl bb nzs ohh kutj e tizkwhce ihvopn. Ci xug hb nzs nuqqbr-gmwy jtqq, rnq lj jl ldjfyl awy yeng e nptopq bm vcacd fhi jps wvaqv usrcy, fomiotgc piuxr ged qlzkkey fk uzvvkj iaws uaiaj eykmwues. Gki dldymza etg nrrqoc cilz xa wwosx zt nqh ulehip lmtre ko fomfl oaj mhg fcsvng. Kmu taic xyol, dokisxpnf tcmz wwfdgieq zmvo tci echx cl khr uiua oa lug, etjku aoryv oegtxyalze rs uh pqvkzh. \"Ibim'g nrpchrgk tj qq?\" bm mvulguw. Mv danr'f u lksgd. Hvv vqvm, v tdixxf nlmnq vqvm vpfbwnun r lvwxnl tjs egiez, rry cheelfppxs jxhcvea lxu mopv ruubzorr jdpnz. A xsxfmvhofn bi xgetdpq mifdrvs ydc uwrzep icm ct khr wedse - Neymi poy r tedzgsldrs miesydaa - drf hbjzq cb mvkie uxri h pdgfozx hnrt uh lck rzgqhbem ilt bxx qm ai mxfclhxrtrg qcnaumzy igr nfufhh ku a imoy, obzjvd sueol. Io wtiexr g caqb jkatzh aob pwzy a sxv jht vrp zck pur wur wca ukvuapm, fgzsvqk c oevzk zck aawf gkev joqidyl mvk nhboi qm hzv xiexf gim gracydn xty dbscvr. Tuiivr olqh bnftvd gr pqvk jyf npx koedbz ev ahz hgft psgkhru. Isy ojtvqzi, kxsa Rukmhn Eeopv rcor tiaq fvbnfzcx swkhps, mi frimq tmryqry xqziucrzsqv mu zxj lot kpcs h hrlpectv tniutu. Rw cks yk hka ausfyy-jzcu eifv, tcd xm xc jiqadz pzc bzjq n mxytim fu bhyuh urf hyd kmcfu hxbvb, oplamnnt fwuqf odr qtyojxz nc coxcsw vbka wfmsy wsanxttz. Whj fegrhas afy tgkhkx vdih bu ogzlj xk kxt ugnqld uyyzz bf quzlp vpx rxs wlmgvt. Koj qhlp duja, stmxfjsbw rhtu bkugklzm jvuw yhb agpd hj clg efsj zo cwv, dgoun dxsxn mynknmaenm qg up ouudap. \"Ajxo'n vecdvzip xb fi?\" vc nwtankt. Nx wdgm'g m hwkms. Amr qjqj, d xxahiy zjdkx hqqv estkisci i cgcktp ayg jwuvi, lcg phgtimscdo emwhxtn xai dmuc mziqcsum fnymh. F cltjubmmxr qs uenergs bhsibov heb mulgvf wcf qb jvr edhrx - Omqux rvg e gfrhixpvgk gyftxshq - ash aecur ux ynqxx ltmb c mlkzgji azpk ro xcf aijeqnju dck mdk wq hx acvocqrcbeg srkhxzfu dvg shjstk yl y ntjd, cqcnyy oennt. Nt ppmmdw e uefl gijeny cda cbjr d byu bfn cif ncd dcq kuz vgz nldmieo, momfwes e tinoc tsl bzlm whfx crjdeqh ynq caskd jh ehz raoiy sgd dymctmw ahh pgaxmi. Eavozy dzvx neongl tr rfsr mll jkm ztgsol hj rfe obkh evkncne.",
            16
        }
    };

    int acertos = 0;

    cout << "========================================\n";
    cout << "INICIANDO TESTES\n";
    cout << "========================================\n\n";

    for (const auto& teste : testes) {
        cout << "Testando: " << teste.nome << '\n';
        
        // Chama a sua função
        vector<int> estimativas = estima_tamanho_chave(teste.texto_cifrado, MAX_TAMANHO);
        
        if (estimativas.empty()) {
            cout << "  [ERRO] A funcao não retornou nenhuma estimativa.\n\n";
            continue;
        }

        int melhor_estimativa = estimativas[0];

        if (melhor_estimativa == teste.tamanho_real_chave) {
            cout << "  [PASSOU] O algoritmo acertou! Tamanho esperado: " << teste.tamanho_real_chave << '\n';
            acertos++;
        } else {
            cout << "  [FALHOU] Tamanho esperado: " << teste.tamanho_real_chave 
                 << " | O algoritmo estimou: " << melhor_estimativa << '\n';
            
            // Mostra onde a resposta certa ficou no ranking, caso tenha errado
            auto it = find(estimativas.begin(), estimativas.end(), teste.tamanho_real_chave);
            if (it != estimativas.end()) {
                int posicao = distance(estimativas.begin(), it) + 1;
                cout << "  (A resposta correta era a " << posicao << "a opcao mais provavel no vetor)\n";
            }
        }
        cout << "----------------------------------------\n";
    }

    cout << "\nRESULTADO FINAL: " << acertos << " de " << testes.size() << " testes passaram.\n";

    return 0;
}