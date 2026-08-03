"""Shared PyTorch model definitions used across the Part 1, 2, 3 and 6 notebooks.

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
