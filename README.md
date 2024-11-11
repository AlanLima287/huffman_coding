# Estruturas de Dados

## Código de Huffman

Dado um alfabeto $`\Sigma = \{c_0, c_1, c_2, \dots, c_{n - 1}\}`$ de tamanho $`n`$, cada $`c_k`$ é chamado caractere. Cada caractere é associado com uma frequência

## Lógica Bit-a-Bit

### Definição A: Bit

Um bit é a unidade mínima de informação, podendo assumir um de dois valores, sim ou não, ligado ou deslidado, verdadeiro ou falso, um ou zero. Aqui, o conjunto de possíveis valores para um bit são definidos como $`\{0,1\}`$

### Definição B: Palavra Binária

Uma palavra binária é uma $`k`$-túpla ordenada $`\omega = \langle \omega_0 \omega_1 \omega_2 \cdots \omega_{k - 1} \rangle`$ de bits, ou seja, $`\omega_n \in \{0,1\}`$. Uma palavra binária que é uma $`k`$-túpla é chamada palavra de $`k`$-bits. Palavras vazias, ou seja, com $`k = 0`$, não são consideradas palavras binárias, pois não carregam informação. Para claridade, espaçamento interno é permitido, exemplo, $`\langle 1010 ~ 0111 ~ 1001 \rangle`$ é mais legível que $`\langle 101001111001 \rangle`$.

O $`n`$-ésimo bit de uma palavra $`k`$-bits $`\omega`$ ($`0`$-ésimo bit é o primeiro bit e $`(k - 1)`$-ésimo bit é o último bit) é dado por $`\omega_n`$, se $`n \le k`$, então $`\omega_n = 0`$. Uma sub-palavra binária do $`n`$-ésimo bit até o $`m`$-ésimo bit, denotada $`\omega_{n..m}`$, é dada $`\omega_{n..m} = \langle \omega_n \omega_{n + 1} \omega_{n + 2} \cdots \omega_{m - 1} \omega_{m} \rangle`$, caso $`n = 0`$, $`\omega_{n..m}`$ pode ser denotado $`\omega_{..m}`$, analogamente, caso $`m = k`$, $`\omega_{n..m}`$ pode ser denotado $`\omega_{n..}`$.

### Definição C: Zero

A palavra binária zero $`\mathbf{0}`$ é definida como uma k-túpla arbitrária (seu tamanho é dependente do contexto) tal que todo bit é $`0`$, $`\mathbf{0} = \langle 000 \cdots 0 \rangle`$.

### Definição D: Operações Bit-a-Bit

#### i) Negação

_Negação_, _inversão lógica_ ou _complemento de um_, $` \mathord\sim : \{0,1\}^k \longrightarrow \{0,1\}^k`$, é definido:

* $`\mathord\sim b = 1 \iff b = 0`$, para $`b \in \{0,1\}`$;
* $`\mathord\sim \langle \omega, b \rangle = \langle \mathord\sim \omega, \mathord\sim b \rangle`$, para $`\omega \in \{0,1\}^{k - 1}`$ e $`b \in \{0,1\}`$.

#### ii) Conjunção

_Conjunção_, ou _e lógico_, $` \mathbin\& : \{0,1\}^k \times \{0,1\}^k \longrightarrow \{0,1\}^k `$, é definido:

* $`a \mathbin\& b = 1 \iff a = b = 1`$, para $`a, b \in \{0,1\}`$;
* $`\langle \omega, a \rangle \mathbin\& \langle \psi, b \rangle = \langle \omega \mathbin\& \psi, a \mathbin\& b \rangle`$, para $`\omega, \psi \in \{0,1\}^{k - 1}`$ e $`a, b \in \{0,1\}`$.

#### iii) Disjunção

_Disjunção_, ou _ou lógico_, $`\mid ~: \{0,1\}^k \times \{0,1\}^k \longrightarrow \{0,1\}^k `$, é definido:

* $`a \mid b = 0 \iff a = b = 0`$, para $`a, b \in \{0,1\}`$;
* $`\langle \omega, a \rangle \mid \langle \psi, b \rangle = \langle \omega \mid \psi, a \mid b \rangle`$, para $`\omega, \psi \in \{0,1\}^{k - 1}`$ e $`a, b \in \{0,1\}`$.

#### iv) Disjunção Exclusiva

_Disjunção exclusica_, _XOR_, _não eqivalência_, ou _ou exclusivo_, $`\oplus : \{0,1\}^k \times \{0,1\}^k \longrightarrow \{0,1\}^k`$, é definido:

* $`a \oplus b = 1 \iff a \ne b`$, para $`a, b \in \{0,1\}`$;
* $`\langle \omega, a \rangle \oplus \langle \psi, b \rangle = \langle \omega \oplus \psi, a \oplus b \rangle`$, para $`\omega, \psi \in \{0,1\}^{k - 1}`$ e $`a, b \in \{0,1\}`$.

#### v) Deslocamento esquerdo

_Deslocamento esquerdo_, ou _left shift_, $`\gg: \{0,1\}^k \times \text{ℕ} \longrightarrow \{0,1\}^k`$, é definido:

* $`\omega \gg 0 = \omega`$;
* $`\langle \omega, b \rangle \gg n = \langle 0, \omega \rangle \gg n - 1`$, para $`\omega \in \{0,1\}^{k - 1}`$ e $`b \in \{0,1\}`$.

#### vi) Deslocamento direito

_Deslocamento direito_, ou _right shift_, $`\ll: \{0,1\}^k \times \text{ℕ} \longrightarrow \{0,1\}^k`$, é definido:

* $`\omega \ll 0 = \omega`$;
* $`\langle b, \omega \rangle \ll n = \langle \omega, 0 \rangle \ll n - 1`$, para $`\omega \in \{0,1\}^{k - 1}`$ e $`b \in \{0,1\}`$.

### Definição E: Operações Compactantes

Operações compactantes são predicados que reduzem uma palavra de $`k`$-bits para um valor de verdade, $`True`$ e $`False`$. Esses operam juntamente a suas versões analogas na lógica classica, inclusive usadando notação comumente usadas nesta.

Primeiro define-se o predicado $`I`$, chamado de interpretação, definido $`I[\omega] \iff \exists n; \omega_n = 1`$.

#### i) Afirmação

A afimação, $`()`$, é definido, sendo $`\omega \in \{0,1\}^k`$:

* $`(\omega) \iff I[\omega]`$

A afirmação se aplica, do ponto de vista simbólico, se, e somente se o invólucro $`()`$ for o mais externo em relação à expressão bit-a-bit, pois valores de verdade não são operáveis, diretamente, com palavras binárias.

#### i) Negação

O predicado negação, $`\lnot`$, é definido, sendo $`\omega \in \{0,1\}^k`$:
* $` \lnot \omega \iff \lnot I[\omega]`$.

#### i) Conjunção

O predicado conjunção, $`\land`$, é definido, sendo $`\omega, \psi \in \{0,1\}^k`$:
* $` \omega \land \psi \iff I[\omega] \land I[\psi] `$.

#### i) Disjunção

O predicado disjunção, $`\lor`$, é definido, sendo $`\omega, \psi \in \{0,1\}^k`$:
* $` \omega \lor \psi \iff I[\omega] \lor I[\psi] `$.

### Usando as Operações Bit-a-Bit

Algumas formas, quando interpretadas, vão gerar a mesma interpretação, como por exemplo, $`(\omega \mid \psi) \iff \omega \lor \psi`$, entretando, para a conjunção é válido apenas $`(\omega \mathbin\& \psi) \implies \omega \land \psi`$. A comparação, $`=`$, possui uma forma equivalente, $`\omega = \psi \iff \lnot (\omega \oplus \psi)`$, que pode ser usado para simplicar a classica comparação da forma $`\omega \mathbin\& \psi = \psi`$, da seguinte forma:

```math
\begin{align*}
   \omega \mathbin\& \psi = \psi 
      &\iff \lnot ((\omega \mathbin\& \psi) \oplus \psi) \\
      &\iff \lnot (((\omega \mathbin\& \psi) \mathbin\& \mathord\sim\psi) \mid (\mathord\sim(\omega \mathbin\& \psi) \mathbin\& \psi)) \\
      &\iff \lnot ((\omega \mathbin\& \psi \mathbin\& \mathord\sim\psi) \mid ((\mathord\sim\omega \mid \mathord\sim\psi) \mathbin\& \psi)) \\
      &\iff \lnot ((\omega \mathbin\& \mathbf{0}) \mid ((\mathord\sim\omega \mathbin\& \psi) \mid (\mathord\sim\psi \mathbin\& \psi))) \\
      &\iff \lnot (\mathbf{0} \mid (\mathord\sim\omega \mathbin\& \psi) \mid \mathbf{0}) \\
      &\iff \lnot (\mathord\sim\omega \mathbin\& \psi) \\
\end{align*}
```

Há bastante informação nessa fórmula, como porque aquela disjunção exclusiva expandiu daquela forma? Porque conjunções com $`\mathbf{0}`$ geram $`\mathbf{0}`$ enquanto disjunção com esse não tem efeito? Essas e outras questões serão respondidas, mas antes, mais dúvidas serão geradas, esperançosamente, havia mais uma forma de derivar aquele resultado:

```math
\omega \mathbin\& \psi = \psi
   \iff \lnot ((\omega \mathbin\& \psi) \oplus \psi)
   \iff \lnot ((\omega \mathbin\& \psi) \oplus (\mathord\sim\mathbf{0} \mathbin\& \psi))
   \iff \lnot ((\omega \oplus \mathord\sim\mathbf{0}) \mathbin\& \psi)
   \iff \lnot (\mathord\sim\omega \mathbin\& \psi)
```

### Teorema da Extensionalidade Bit-a-Bit: Se uma propriedade bit-a-bit é válida para bits únicos, então será válida para palavras binárias de tamanho arbitrário

Uma propriedade bit-a-bit é definida. Sejam $`P(\omega_0, \omega_1, \dots, \omega_t)`$, $`\omega_i \in \{0,1\}^k; \forall i \le t `$ e $`Q(b_0, b_1, \dots, b_t)`$, $`b_i \in \{0,1\}; \forall i \le t `$ predicados. O predicado $`Q`$ tem uma interpretação qualquer e o predicado $`P`$ possui interpretação da seguinte forma, sendo, $`\forall i \le t`$, $`\psi_i \in \{0,1\}^k`$ e $`b_i \in \{0,1\}`$:

* $`P(\langle \psi_0, b_0 \rangle, \langle \psi_1, b_1 \rangle, \dots, \langle \psi_t, b_t \rangle) \iff P(\psi_0, \psi_1, \dots, \psi_t) \land Q(b_0, b_1, \dots, b_t)`$;
* $`P(b_0, b_1, \dots, b_t) \iff Q(b_0, b_1, \dots, b_t)`$.

A interpretação de $`P`$ é uma consequência direta da interpretação de $`Q`$, assim, por indução, usando $`P`$ e $`Q`$ definidos anteriormente. Para o passo base, palavra binárias serão bits nessa configuração, logo, pela definição, $`P`$ é equivalente a $`Q`$. Para o passo indutivo, assume-se $`P(\psi_0, \psi_1, \dots, \psi_t)`$ é válido, para seu sucessor tem-se: $`P(\langle \psi_0, b_0 \rangle, \langle \psi_1, b_1 \rangle, \dots, \langle \psi_t, b_t \rangle) \iff P(\psi_0, \psi_1, \dots, \psi_t) \land Q(b_0, b_1, \dots, b_t) \iff Q(b_0, b_1, \dots, b_t)`$. Assim, a validade da propriedade $`P`$ é equivale a validade da propriedade para bits único, ou seja, à validade de $`Q`$.

### Corolário F: Propriedades Bit-a-Bit

Pelo _Teorema da Extensionalidade Bit-a-Bit_, as operações discutidas serão provadas para bits individuais, partindo diretamente de suas definições.

#### i) Comutatividade

A conjunção, disjunção e disjunção exclusiva bit-a-bit são comutativas, sejam $`a, b \in \{0,1\}`$:

* $`a \mathbin\& b = 1 \iff a = b = 1 \iff b = a = 1 \iff b \mathbin\& a = 1`$;
* $`a \mid b = 0 \iff a = b = 0 \iff b = a = 0 \iff b \mid a = 0`$;
* $`a \oplus b = 1 \iff a \ne b \iff b \ne a \iff b \oplus a = 1`$.

#### ii) Associatividade

A conjunção, disjunção e disjunção exclusiva bit-a-bit são associativas, sejam $`a, b, c \in \{0,1\}`$:

* $`(a \mathbin\& b) \mathbin\& c = 1 \iff (a \mathbin\& b) = c = 1 \iff a = b = c = 1 \iff a = (b \mathbin\& c) = 1 \iff a \mathbin\& (b \mathbin\& c) = 1`$;
* $`(a \mid b) \mid c = 0 \iff (a \mid b) = c = 0 \iff a = b = c = 0 \iff a = (b \mid c) = 0 \iff a \mid (b \mid c) = 0`$;

Para a disjunção exclusiva, seque a tabela:

| $`a`$ | $`b`$ | $`c`$ | $`a \oplus b`$ | $`(a \oplus b) \oplus c`$ | $`b \oplus c`$ | $`a \oplus (b \oplus c)`$ |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| $`0`$ | $`0`$ | $`0`$ | $`0`$ | $`0`$ | $`0`$ | $`0`$ |
| $`0`$ | $`0`$ | $`1`$ | $`0`$ | $`1`$ | $`1`$ | $`1`$ |
| $`0`$ | $`1`$ | $`0`$ | $`1`$ | $`1`$ | $`1`$ | $`1`$ |
| $`0`$ | $`1`$ | $`1`$ | $`1`$ | $`0`$ | $`0`$ | $`0`$ |
| $`1`$ | $`0`$ | $`0`$ | $`1`$ | $`1`$ | $`0`$ | $`1`$ |
| $`1`$ | $`0`$ | $`1`$ | $`1`$ | $`0`$ | $`1`$ | $`0`$ |
| $`1`$ | $`1`$ | $`0`$ | $`0`$ | $`0`$ | $`1`$ | $`0`$ |
| $`1`$ | $`1`$ | $`1`$ | $`0`$ | $`1`$ | $`0`$ | $`1`$ |

Assim, nota-se que as expressões são equivalentes.

#### iii. Distributividade

Há três classes de distributividade:

1. $`(a \mid b) \mathbin\& c = (a \mathbin\& c) \mid (b \mathbin\& c)`$
2. $`(a \mathbin\& b) \mid c = (a \mid c) \mathbin\& (b \mid c)`$
3. $`(a \oplus b) \mathbin\& c = (a \mathbin\& c) \oplus (b \mathbin\& c)`$

Assim, seguem as demonstrações:

```math
\begin{align*}
   (a \mid b) \mathbin\& c = 1 
      &\iff (a \mid b) = c = 1 \\
      &\iff (a = 1 \lor b = 1) \land c = 1 \\
      &\iff a = c = 1 \lor b = c = 1 \\
      &\iff a \mathbin\& c = 1 \lor b \mathbin\& c = 1 \\
      &\iff \lnot(a \mathbin\& c = b \mathbin\& c = 0) \\
      &\iff (a \mathbin\& c) \mid (b \mathbin\& c) \ne 0 \\
      &\iff (a \mathbin\& c) \mid (b \mathbin\& c) = 1
\end{align*}
```

```math
\begin{align*}
   (a \mathbin\& b) \mid c = 0
      &\iff (a \mathbin\& b) = c = 0 \\
      &\iff (a = 0 \lor b = 0) \land c = 0 \\
      &\iff a = c = 0 \lor b = c = 0 \\
      &\iff a \mid c = 0 \lor b \mid c = 0 \\
      &\iff \lnot(a \mid c = b \mid c = 1) \\
      &\iff (a \mid c) \mathbin\& (b \mid c) \ne 1 \\
      &\iff (a \mid c) \mathbin\& (b \mid c) = 0
\end{align*}
```

```math
\begin{align*}
   (a \oplus b) \mathbin\& c = 1
      &\iff (a \mathbin\& b) \ne c \\
      &\iff (a \mathbin\& b) \ne c \\
\end{align*}
```