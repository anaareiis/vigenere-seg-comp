#include <string>
#include <vector>
#include <iostream>

using namespace std;

// Copiei pq não tava conseguindo colocar o include, se conseguir pfv troca pra usar o que já tem
bool is_upper_ascii(const unsigned char value) {return value >= static_cast<unsigned char>('A') && value <= static_cast<unsigned char>('Z');}
bool is_lower_ascii(const unsigned char value){return value >= static_cast<unsigned char>('a') && value <= static_cast<unsigned char>('z');}

/**
 * @brief Estima o tamanho da chave de um texto cifrado com a Cifra de Vigenère, utilizando o Método do Índice de Coincidência.
 * @param CYPHERTEXT O texto cifrado. Caracteres não-alfa serão ignorados. Maiúsculas e minúsculas não são diferenciadas.
 * @param MAX_TAMANHO_CHAVE O tamanho máximo que a chave pode ter. Todos os tamanhos de 1 até esse valor serão testados.
 * @return Um vetor de inteiros com os valores prováveis do tamanho da chave, em ordem descrescente de probabilidade. Ou seja, o primeiro elemento é o mais provável.
 * Caso o cyphertext seja suficientemente grande, o primeiro elemento é quase certamente o tamanho da chave.
 */
vector<int> estima_tamanho_chave(
    const string &CYPHERTEXT,
    const int MAX_TAMANHO_CHAVE
) {
    int tamanho_cyphertext = CYPHERTEXT.length();
    
    // Remove caracteres não-alfa e transforma as letras em maiúsculas
    string cyphertext_limpo = "";
    for (int i = 0; i < tamanho_cyphertext; i++) {
        char caracter = CYPHERTEXT[i];
        
        if (!is_lower_ascii(caracter) && !is_upper_ascii(caracter))
        continue;
        
        if (is_lower_ascii(caracter))
        cyphertext_limpo += caracter - 32;
        else
        cyphertext_limpo += caracter;
    }
    int tamanho_cyphertext_limpo = cyphertext_limpo.length();


    double maior_ioc_medio = -1;
    int estimativa_tamanho = -1;
    vector<pair<double,int>> ioc_tamanho;
    
    // Itera pelos tamanhos possívieis, até o tamanho máximo
    for (int tamanho_testado = 1; tamanho_testado <= MAX_TAMANHO_CHAVE; tamanho_testado++) {

        // Divide o cyphertext em grupos, 
        vector<string> grupos;
        for (int index_grupo = 0; index_grupo < tamanho_testado; index_grupo++) {

            int index = index_grupo;
            string grupo = "";

            while (index < tamanho_cyphertext_limpo) {
                grupo += cyphertext_limpo[index];
                index += tamanho_testado;
            }
            
            grupos.push_back(grupo);
        }

        // Cálculo do IoC médio para o tamanho testado
        double ioc_medio = 0.0;        
        for (string grupo : grupos) {
            
            // Calcula a frequência de cada letra
            vector<int> frequencias(26, 0);
            for (char c : grupo) {
                int i = c - 'A';
                frequencias[i]++;
            }

            // Cálculo do IoC do grupo
            double ioc = 0.0;
            for (int i = 0; i < 26; i++) {
                ioc += frequencias[i] * (frequencias[i] - 1);
            }
            ioc /= grupo.length() * (grupo.length() - 1);
            ioc_medio += ioc;
            
        }
        
        // Atualiza a estimativa, se o IoC médio for maior que o da estimativa anterior
        ioc_medio = ioc_medio / grupos.size();
        if (ioc_medio > maior_ioc_medio) {
            maior_ioc_medio = ioc_medio;
            estimativa_tamanho = tamanho_testado;
        }

    }

    return estimativa_tamanho;
}

// Teste
int main() {

    int resultado_teste = estima_tamanho_chave(
        (
            // Tamanho da chave: 6
            "Dlb X fiuk mmizczv ih mql pdp onc bwbg ozaityge qsxo qw ltgcwekxgu rcmplitv icw dtrqhbbi gixg kcj jdkb cel X pwnc oxos afc p vcogttms ctkdnbv fn ias upaixa, cel tqdqlvs mvg rkinon kmpvvkeoh ht vym vksck mmizqimg ht vym ikivy, bwx acjbtk-pwztsxf qw pjfop yieiwpvah. Gc qem gxxgtbh, wwucqzxg, qi ikhwfj xaxoulzt bhuvtu, userchx wv za eescjcgx, pwk jtvowjm iacuv ewh rq ewi dbqn pdp hq gcglig gtttgwim gthkfvpeza vvrhipkmg vcpjmfnsptmh mvck igx szkztfsnp xpbbhlt. Chf cxixg wu kptks cegdgs yyw ahjgj wg iitjctl ct umhbfgj bd hpvrqc ioke wu bhuvtu, userchx wv za etwp, sci userchx cetihbcprtar qkikjfgvrvrxg qtkjk wp npxvv vfqa tbf gixg qce xghqwim wba ufut zfgrb eescjcgx. Hq kizx o viqkbon vfpfdnv, ewbqj fn jl sxvz jgrgibpdsu ciqhfkfch ivajqrtz gomgvwuv, mmvsrk bd hpvrqc lcov isoopkivx ttfu xm? Pwk ewh vcj icr fkxpi mc hzvs yowcb lbhj r upg kjf kwhcuva ih spawn t dnvihnfg kppm vcj vd tbpfgxgu efvhxewvvrxg, qi wcx kjf ikhwfj i etwp kppm dtfljvsu ew gxgwcbpgh rcmplitv? Wc mvg fbwxf jrvs, ps fvvdnbev exmv tzowmsqla xgrkxvpmwqe icw rkjtxds ovv lac cim hh pgxcxesf rvs wsofzpewbvl qr hjv kwtfoj wu izgrajks qw bwx aqdmcm, gq stxgrgu jn wsuzzt, mvck bwxm ervchh hfztlsg"
        ),
        10
    );

    cout << "O tamanho estimado eh " << resultado_teste << '\n';

    return 0;
}