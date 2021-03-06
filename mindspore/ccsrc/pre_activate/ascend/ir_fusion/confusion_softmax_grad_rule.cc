/**
 * Copyright 2020 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "pre_activate/ascend/ir_fusion/confusion_softmax_grad_rule.h"

#include <memory>
#include <vector>

#include "session/anf_runtime_algorithm.h"
#include "ir/primitive.h"
#include "utils/utils.h"

namespace mindspore {
namespace opt {
const BaseRef ConfusionSoftmaxGradRule::DefinePattern() const {
  return VectorRef(
    {prim::kPrimSub, input0_, VectorRef({prim::kPrimReduceSum, VectorRef({prim::kPrimMul, input0_, input1_})})});
}

const AnfNodePtr ConfusionSoftmaxGradRule::Process(const FuncGraphPtr &graph, const AnfNodePtr &node,
                                                   const EquivPtr &equiv) const {
  MS_EXCEPTION_IF_NULL(graph);
  MS_EXCEPTION_IF_NULL(node);
  MS_EXCEPTION_IF_NULL(equiv);
  auto input0 = utils::cast<AnfNodePtr>((*equiv)[input0_]);
  auto input1 = utils::cast<AnfNodePtr>((*equiv)[input1_]);
  MS_EXCEPTION_IF_NULL(input0);
  MS_EXCEPTION_IF_NULL(input1);

  auto prim = std::make_shared<Primitive>(kConfusionSoftmaxGradOpName);
  MS_EXCEPTION_IF_NULL(prim);
  std::vector<AnfNodePtr> inputs = {NewValueNode(prim), input0, input1};
  auto confusion_softmax_grad = graph->NewCNode(inputs);
  MS_EXCEPTION_IF_NULL(confusion_softmax_grad);
  auto types = {AnfAlgo::GetOutputInferDataType(node, 0)};
  auto shapes = {AnfAlgo::GetOutputInferShape(node, 0)};
  AnfAlgo::SetOutputInferTypeAndShape(types, shapes, confusion_softmax_grad.get());
  confusion_softmax_grad->set_scope(node->scope());
  return confusion_softmax_grad;
}
}  // namespace opt
}  // namespace mindspore
