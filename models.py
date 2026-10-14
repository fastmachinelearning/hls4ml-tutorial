"""Shared model definitions used across the Part 1, 2, 3, 5 and 6 notebooks.

Keeping these in one place means the architecture only needs to be changed here,
rather than in every notebook that loads a model trained in `1b_train_pytorch.ipynb`.
"""

import torch
import torch.nn as nn


class JetTagger(nn.Module):
    """Simple 3-hidden-layer jet tagger: 16 -> 64 -> 32 -> 32 -> 5."""

    def __init__(self):
        super().__init__()
        self.fc1 = nn.Linear(16, 64)
        self.fc2 = nn.Linear(64, 32)
        self.fc3 = nn.Linear(32, 32)
        self.output = nn.Linear(32, 5)

    def logits(self, x):
        x = torch.relu(self.fc1(x))
        x = torch.relu(self.fc2(x))
        x = torch.relu(self.fc3(x))
        return self.output(x)

    def forward(self, x):
        return torch.softmax(self.logits(x), dim=1)


class JetTaggerBrevitas(nn.Module):
    """Quantized (6-bit) counterpart of `JetTagger`, built with Brevitas layers."""

    def __init__(self):
        super().__init__()
        import brevitas.nn as qnn

        self.fc1 = qnn.QuantLinear(16, 64, bias=False, weight_bit_width=6)
        self.relu1 = qnn.QuantReLU(bit_width=6)
        self.fc2 = qnn.QuantLinear(64, 32, bias=False, weight_bit_width=6)
        self.relu2 = qnn.QuantReLU(bit_width=6)
        self.fc3 = qnn.QuantLinear(32, 32, bias=False, weight_bit_width=6)
        self.relu3 = qnn.QuantReLU(bit_width=6)
        self.output = qnn.QuantLinear(32, 5, bias=False, weight_bit_width=6)

    def logits(self, x):
        x = self.relu1(self.fc1(x))
        x = self.relu2(self.fc2(x))
        x = self.relu3(self.fc3(x))
        return self.output(x)

    def forward(self, x):
        return torch.softmax(self.logits(x), dim=1)


def create_simple_unet(input_shape=(4, 4, 1)):
    """Tiny Keras U-Net with one skip connection, used by the Part 5 (Vitis Unified) notebooks.

    The model is intentionally small so that C simulation, RTL co-simulation and bitfile generation
    finish in a reasonable time. The weights are random: these notebooks check the *flow*, not the accuracy.
    """
    import keras
    from keras.layers import Concatenate, Conv2D, Input, MaxPooling2D, UpSampling2D

    inputs = Input(input_shape)
    # Encoder
    c1 = Conv2D(2, (3, 3), activation='relu', padding='same')(inputs)
    p1 = MaxPooling2D((2, 2))(c1)
    # Bottleneck
    bn = Conv2D(4, (3, 3), activation='relu', padding='same')(p1)
    # Decoder with skip connection
    u1 = UpSampling2D((2, 2))(bn)
    concat1 = Concatenate()([u1, c1])
    c2 = Conv2D(2, (3, 3), activation='relu', padding='same')(concat1)
    # Output layer (1 channel)
    outputs = Conv2D(1, (1, 1), activation='sigmoid')(c2)
    model = keras.Model(inputs, outputs)
    model.compile(optimizer='adam', loss='binary_crossentropy')
    return model
