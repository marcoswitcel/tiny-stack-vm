# Pendências

## Objetivos gerais

- [x] Implementar um conjunto mínimo de opcodes da ISA
- [x] Parsear e converter programas de exemplo em para bytecode
- [x] Rodar bytecode
- [ ] Programas de exemplos para testar a VM (apenas um na primeira)

## Na lista de desenvolvimento

- [x] Ajustar o parser para suportar operadores no formato de caractere literal, exemplo: '*' para o código do asterísco na tabela ascii
- [ ] Ajustar o parser para suportar referencias a labels no campo operador e ajustar para gerar o bytecode apropriado
- [ ] Analisar e adicionar mais instruções a VM, implementar formas de incremento e decremento e o que mais for interessante para o uso geral e performático (pesquisar)
- [x] Implementar o suporte ao comando para invocar um trecho de código (chamar função) e retornar para o lugar da invocação (callsite)
- [ ] Definir a lista de instruções e seus possíveis parâmetros (formalização)
- [ ] Melhorar a forma de retornar sinais da VM. Trazer mais informações contextuais (instrução executada e operando)
- [ ] Resolver ambiguidade na hora de identificar se o programa terminou por halt, por não ter mais instruções para executar ou por atingir o limite de ticks especificados
- [ ] Melhorar os programas de teste da suite de teste da VM, revisar para ter pelo menos um teste por instrução
- [ ] Melhorar a suite de teste do assembler, muito incompleta atualmente
- [ ] Melhorar as informações providas pelo assembler quando o programa contém algum erro sintático

## Bugs
- [x] Char literal não está aceitando espaço
- [ ] o comando "CALL 1" quando na última linha e sem espaço ou enter após está retornado "esperava um número" ao invés de parsear corretamente


https://gist.github.com/Lewkos-snippets/3873832
https://www.tecmint.com/best-hex-editors-for-linux/
https://stackoverflow.com/questions/14002954/c-programming-how-to-read-the-whole-file-contents-into-a-buffer
https://stackoverflow.com/questions/4879286/specifying-size-of-enum-type-in-c
https://stackoverflow.com/questions/41328483/how-to-save-arraybuffer
https://transang.me/edit-binary-file-with-vim-and-the-xxd-command/
https://earthly.dev/blog/make-flags/