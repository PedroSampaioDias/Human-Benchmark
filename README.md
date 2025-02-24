# Human Benchmark

## Descrição do Projeto

O "Human Benchmark" é um sistema embarcado desenvolvido para medir o tempo de reação de usuários a estímulos visuais e auditivos. Utilizando um microcontrolador Raspberry Pi Pico W, o dispositivo apresenta estímulos através de um display OLED SSD1306, LEDs Neopixel e um buzzer piezoelétrico. A interação do usuário é capturada por meio de botões tácteis, e o tempo de resposta é calculado e exibido em tempo real.

## Objetivo

O principal objetivo deste projeto é fornecer uma ferramenta precisa e acessível para avaliar a rapidez com que uma pessoa responde a determinados sinais. Além de servir como um meio de autoavaliação, o dispositivo pode ser utilizado em contextos clínicos para monitorar reflexos em indivíduos com condições neurológicas que afetam o tempo de reação.

## Funcionalidades

- **Medição do Tempo de Reação**: Calcula o intervalo entre a apresentação do estímulo e a resposta do usuário.
- **Estímulos Multissensoriais**: Utiliza sinais visuais (LEDs e display) e auditivos (buzzer) para os testes.
- **Feedback Imediato**: Exibe o tempo de reação no display OLED imediatamente após a resposta.
- **Interface Intuitiva**: Operação simples através de botões tácteis, com instruções claras no display.

## Componentes Utilizados

- **Raspberry Pi Pico W**: Microcontrolador central do sistema.
- **Display OLED SSD1306**: Tela para exibição de instruções e resultados.
- **Botões Tácteis**: Capturam as respostas e interações do usuário.
- **Buzzer Piezoelétrico**: Emite sinais sonoros durante os testes.
- **LEDs Neopixel**: Fornecem estímulos visuais adicionais.
- **Microfone Analógico**: Utilizado para gerar a semente do gerador randômico, garantindo variação nos intervalos de estímulo.

## Aplicações

Este dispositivo é útil para indivíduos que desejam avaliar e melhorar seus tempos de reação, bem como para profissionais de saúde que necessitam monitorar reflexos em pacientes com condições que afetam o sistema neuromuscular.

## Instruções de Uso

1. **Inicialização**: Conecte o dispositivo a uma fonte de energia adequada.
2. **Início do Teste**: Pressione o botão designado para iniciar o teste.
3. **Aguarde o Estímulo**: Após uma contagem regressiva, um estímulo visual ou auditivo será apresentado.
4. **Responda ao Estímulo**: Pressione o botão de resposta o mais rápido possível após perceber o estímulo.
5. **Visualize o Resultado**: O tempo de reação será exibido no display OLED.
6. **Repetição**: Para realizar um novo teste, repita os passos acima.

## Considerações Finais

O "Human Benchmark" combina hardware acessível com software eficiente para criar uma ferramenta útil na avaliação de tempos de reação. Seja para uso pessoal ou profissional, este dispositivo oferece uma maneira prática de monitorar e melhorar os reflexos humanos. 