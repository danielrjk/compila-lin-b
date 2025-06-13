Trabalho feito para a disciplina de Software Básico

Daniel Rodrigues Jacinto Kielmanowicz
2310703		Turma: 3WC

Relatório Trabalho 2

Este relatório descreve o desenvolvimento e o estado atual do compilador para a linguagem LinB. O objetivo do projeto foi implementar um compilador capaz de traduzir programas escritos em LinB para código de máquina x86-64. Abaixo estão detalhadas as funcionalidades que foram implementadas com sucesso, bem como as limitações encontradas durante o desenvolvimento.

Instruções de Atribuição:

As instruções de atribuição foram implementadas com suporte para variáveis (v1, v2, ...) e constantes ($num).
	Exemplo:
	linb
		v1 <= $10 + $5
	
	Código de máquina gerado
		48 C7 C0 0F 00 00 00
	
Instrução de Retorno:
Implementação da instrução ret para finalizar a execução da função.
	Exemplo:
	linb
		ret
		
	Código de máquina gerado
		C3	
	
Desvio Condicional (if):
Implementação básica do desvio condicional utilizando jne (jump if not equal).
	Exemplo:
	linb
		if v1 3
		v1 <= v1 + $1
		ret
	
	Código de máquina gerado
		48 83 f8 00 0f 85 01 00 00 00 48 83 c0 01 c3 
	
Funcionalidades Não Implementadas
    Otimizações Avançadas:
        Não foi implementada nenhuma forma de otimização de código ou análise de fluxo.

    Tratamento Completo de Erros:
        O compilador não trata todas as situações de erro possíveis, como uso de variáveis não declaradas.
        

        
Para os testes, utilizei os códigos fornecidos e um main.c para teste, a seguinte linha de comando foi executada no terminal:
	gcc -Wall-Wa,--execstack -o main main.c compilaLinB.c && ./main
	
Exemplos de teste:
	Exemplo 1:
		v1 <= p1 + $1
		ret
	
	Código de máquina gerado:
		48 89 f8 48 83 c0 01 c3
		
	Chamada pela main:
		funcao = compilaLinB(f, codigo);
		int resultado = funcao(5);
		printf("Resultado: %d\n\n", resultado);
		
	Resultado:
		6
		
	Exemplo 2:
		v1 <= p1 + p2
		p1 <= p1 - p2
		v1 <= v1 * p1
		ret
		
	Código de máquina gerado:
		48 89 f8 48 01 f0 48 29 f7 48 0f af c7 c3 

	Chamada pela main:
		funcao = compilaLinB(f, codigo);
		int resultado = funcao(5, 3);
		printf("Resultado: %d\n\n", resultado);
		
	Resultado:
		16
		
	Exemplo 3:
		v1 <= $0 + $1
		if p1 4
		ret
		v1 <= v1 * p1
		p1 <= p1 - $1
		if p1 4
		ret
	
	Código de máquina gerado:
		48 c7 c0 01 00 00 00 48 83 ff 00 0f 85 01 00 00 
		00 c3 48 0f af c7 48 83 ef 01 48 83 ff 00 0f 85 
		ee ff ff ff c3 

	Chamada pela main:
		funcao = compilaLinB(f, codigo);
		int resultado = funcao(5);
		printf("Resultado: %d\n\n", resultado);
		
	Resultado:
		120
		
	Exemplo 4:
		v1 <= $0 + $0
		v2 <= $1 + $0
		p1 <= p1 + $1
		v3 <= p1 - v2
		if v3 7
		ret
		v3 <= v2 * v2
		v1 <= v1 + v3
		v2 <= v2 + $1
		if v2 4
	
	Código de máquina gerado:
		48 c7 c0 00 00 00 00 48 c7 c3 01 00 00 00 48 83 
		c7 01 48 89 f9 48 29 d9 48 83 f9 00 0f 85 04 00 
		00 00 c3 48 89 d9 48 0f af cb 48 01 c8 48 83 c3 
		01 48 83 fb 00 0f 85 d9 ff ff ff 


	Chamada pela main:
		funcao = compilaLinB(f, codigo);
		int resultado = funcao(5);
		printf("Resultado: %d\n\n", resultado);
		
	Resultado:
		55
	
Conclusão

O compilador LinB apresenta funcionalidades básicas implementadas e funcionais para tradução de programas simples escritos nesta linguagem. No entanto, ainda há espaço para melhorias significativas, especialmente no que diz respeito à detecção e tratamento de erros mais complexos e à implementação de otimizações no código gerado.
