# 🪑 Smart Desk: Monitoramento Ergonômico Inteligente

**Documentação do Challenge Sprint - FIAP**

**Grupo CATECH**

## 👨‍💻 Integrantes

-   **Daniel Santana Corrêa Batista** `[RM559622]`

-   **Wendell Nascimento Dourado** `[RM559336]`

-   **Jonas de Jesus Campos de Oliveira** `[RM561144]`


----------

## 1. 🎯 Tema e Problema

Com o aumento do trabalho remoto e híbrido, a saúde ocupacional tornou-se uma preocupação crítica. Má postura, iluminação inadequada e longos períodos sentados contribuem para lesões por esforço repetitivo (LER), dores crônicas e baixa produtividade.

O projeto **"Smart Desk"** visa resolver esse problema criando uma estação de trabalho inteligente que monitora ativamente as condições do ambiente e o comportamento do usuário, fornecendo feedback em tempo real para prevenir problemas de saúde a longo prazo.

## 2. 🛠️ A Solução IoT

A solução consiste em um sistema embarcado utilizando **ESP32** e sensores simulados que captam métricas ambientais e comportamentais. O sistema opera de forma autônoma para verificar regras de ergonomia e envia os dados para nuvem para histórico e alertas.

### Regras de Negócio e Alertas

O sistema processa as seguintes condições para gerar alertas:

**Regra**

**Condição Monitorada**

**Título do Alerta**

**Grau de Severidade**

**1**

Postura inadequada

Postura Inadequada

🔴 ALTO

**2**

Tempo sentado ≥ 60 min

Muito tempo sentado

🟡 MÉDIO

**3**

Luz < 300 lux

Baixa iluminação

🟡 MÉDIO

**4**

Temp < 20°C ou > 27°C

Temperatura desconfortável

🟠 BAIXO/ALTO

**5**

Tela fora de 100–130 cm

Altura da tela incorreta

🟡 MÉDIO

----------

## 🏗️ Arquitetura Técnica da Solução

O projeto utiliza uma **Arquitetura Híbrida** para garantir redundância e flexibilidade na integração:

1.  **ESP32 (Simulador Wokwi):**

    -   Atua como o controlador central.

    -   Gera dados simulados de forma **gradual (ondas senoidais)** para temperatura, luminosidade, altura e postura, permitindo uma visualização fluida nos gráficos.

    -   Gera um **UUID único** baseado no MAC Address do chip para identificar a mesa.

2.  **Canal 1 - ThingSpeak (Via HTTP REST):**

    -   O ESP32 envia dados a cada 20 segundos via requisição `POST` direta para a API do ThingSpeak.

    -   Objetivo: Armazenamento histórico e visualização rápida em Dashboard (Gráficos).

3.  **Canal 2 - Node-RED (Via MQTT):**

    -   O ESP32 publica simultaneamente um payload JSON no tópico `smartdesk/medicoes` no broker `broker.hivemq.com`.

    -   Objetivo: Integração com fluxos de automação no Node-RED para processamento de regras complexas e envio para outros endpoints/bancos de dados.


----------

## ⚙️ Especificações de Dados

### 1. Mapeamento ThingSpeak (Canais)

Para reproduzir o dashboard, os campos foram configurados da seguinte maneira:

-   **Field 1:** Temperatura (°C)

-   **Field 2:** Iluminação (Lux)

-   **Field 3:** Tempo Sentado (min)

-   **Field 4:** Altura da Tela (cm)

-   **Field 5:** Postura (0=Correta, 1=Inclinada, 2=Curvada)

-   **Field 6:** Device UUID


### 2. Payload MQTT (JSON)

O dado enviado para o Broker MQTT possui a seguinte estrutura para consumo no Node-RED:

JSON

```
{
  "uuid": "AC67B23C9910",
  "temperatura": 24.5,
  "iluminacao": 350,
  "tempo_sentado": 40,
  "altura_tela": 115.0,
  "postura_id": 0,
  "postura_desc": "CORRETA"
}

```

----------

## 🚀 Como Executar o Projeto

1.  **Hardware/Simulação:** Carregar o código `main.cpp` no simulador Wokwi ou ESP32 físico.

2.  **Configuração:** Ajustar as constantes `CHANNEL_ID` e `WRITE_API_KEY` do ThingSpeak no código.

3.  **Monitoramento:**

    -   Acessar o canal do ThingSpeak para ver os gráficos.

    -   Conectar o Node-RED ao broker `broker.hivemq.com` no tópico `smartdesk/medicoes` para ver o fluxo de dados JSON.


### Link do vídeo com protótipo

_(Insira o link do seu vídeo novo aqui)_